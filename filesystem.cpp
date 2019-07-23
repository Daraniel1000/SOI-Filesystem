#include "filesystem.h"
using namespace std;

filesystem::filesystem(string name): _name(name)
{
  _size = boost::filesystem::file_size(_name) / BLOCK_SIZE;
  buffer buf[SYSTEM_BLOCKS];
  ifstream istr;
  istr.open(_name.c_str(), ios::in|ios::binary);
  for(unsigned j=0; j < SYSTEM_BLOCKS; j++)
  {
    istr.read(buf[j], BLOCK_SIZE);
  }
  total_blocks_used = SYSTEM_BLOCKS;
  for(unsigned j=0; j < SYSTEM_BLOCKS; j++)
  {
    for(unsigned i=0; i < BLOCK_SIZE; i+=sizeof(inode))
    {
      inode new_inode = *reinterpret_cast<inode *>(buf[j] + i);
      if(new_inode.used)
      {
        inodes.push_back(new_inode);
        total_blocks_used += new_inode.blocks;
      }
    }
  }
  istr.close();
}

filesystem::filesystem(string name, size_t size): _name(name)
{
  if(size <= SYSTEM_BLOCKS)
  {
    throw std::out_of_range("Podany rozmiar wirtualnego dysku jest za maly");
  }
  _size = size;
  buffer buf;
  for(unsigned i=0; i<BLOCK_SIZE; ++i)
  {
    buf[i] = '\0';
  }
  total_blocks_used = SYSTEM_BLOCKS;
  fstream ostr;
  ostr.open(_name.c_str(), ios::out|ios::binary);
  for(unsigned i=0; i<size; ++i)
  {
    ostr.write(buf, BLOCK_SIZE);
  }
  ostr.close();
}

filesystem::~filesystem()
{
  close();
}

void filesystem::list_files()
{
  cout << setw(10) << "start"
       << setw(10) << "koniec"
       << setw(10) << "rozmiar"
       << setw(10) << "rozmiar"
       << setw(30) << "nazwa" << endl;
  cout << setw(10) << "[blok]"
       << setw(10) << "[blok]"
       << setw(10) << "[bloki]"
       << setw(10) << "[bajty]" << endl;
  for(unsigned i = 0; i < inodes.size(); ++i)
  {
    if(!inodes[i].used) continue;
    cout << setw(10) << inodes[i].begin
         << setw(10) << inodes[i].end()-1
         << setw(10) << inodes[i].blocks
         << setw(10) << inodes[i].size
         << setw(30) << inodes[i].name << endl;
  }
}

void filesystem::defragment()
{
  while(1)
  {
    sort(inodes.begin(), inodes.end(), cmp_inodes);
    unsigned index = 0, new_pos = SYSTEM_BLOCKS;
    // jesli inodes[0].begin != SYSTEM_BLOCKS, to przesuwamy pierwszy plik
    if(inodes[0].begin == SYSTEM_BLOCKS)
    {
        // jesli nie, to szukamy dziury miedzy dwoma plikami
        for(index = 1; index < inodes.size(); ++index)
        {
            if(inodes[index-1].end() < inodes[index].begin)
            {
                new_pos = inodes[index-1].end();
                break;
            }
        }
    //jesli nie ma dziur, koniec defragmentacji
    if(new_pos == SYSTEM_BLOCKS) return;
    }

    unsigned blocks = inodes[index].blocks;
    unsigned old_pos = inodes[index].begin;
    inodes[index].begin = new_pos;
    buffer buffers[blocks];
    ifstream istr;
    istr.open(_name.c_str());
    istr.seekg(old_pos * BLOCK_SIZE);
    for(unsigned i = 0; i < blocks; ++i)
    {
        istr.read(buffers[i], BLOCK_SIZE);
    }
    fstream ostr;
    ostr.open(_name.c_str(), ios::in|ios::out|ios::binary);
    ostr.seekp(new_pos * BLOCK_SIZE);
    for(unsigned i = 0; i < blocks; ++i)
    {
        ostr.write(buffers[i], BLOCK_SIZE);
    }
  }
}

void filesystem::clearmem(unsigned how_many, unsigned offset)
{
  fstream ostr;
  ostr.open(_name.c_str(), ios::in|ios::out|ios::binary);
  ostr.seekg(offset * BLOCK_SIZE);
  buffer buf;
  for(unsigned i=0; i<BLOCK_SIZE; ++i)
  {
    buf[i] = '\0';
  }
  while(how_many--)
  {
    ostr.write(buf, BLOCK_SIZE);
  }
}

void filesystem::remove_file(std::string file_name)
{
  inode *node;
  if((node = find_inode_by_name(file_name)) == NULL)
  {
    cout<<"Podany plik nie istnieje\n";
    return;
  }
  total_blocks_used -= node->blocks;
  // usuwamy inode z tablicy
  swap(*node, inodes.back());
  inodes.pop_back();
}

void filesystem::map()
{
  char tab[_size];
  for(unsigned i = 0; i < SYSTEM_BLOCKS; ++i)
  {
    tab[i] = '+';
  }
  for(unsigned i = SYSTEM_BLOCKS; i < _size; ++i)
  {
    tab[i] = '.';
  }
  for(unsigned j = 0; j < inodes.size(); ++j)
  {
    for(unsigned i = inodes[j].begin; i < inodes[j].end(); ++i)
    {
      tab[i] = inodes[j].name[0];
    }
  }
  cout<< "Legenda: " << endl
      << "  '.' = blok wolny" << endl
      << "  '+' = blok systemowy (zarezerwowany na deskryptory plikow)" << endl
      << "  'w' = blok zajety przez plik (w - pierwsza litera nazwy)" << endl
      << "1 blok = "<< BLOCK_SIZE << " bajty/ow" << endl << endl;

  for(unsigned i = 0; i < _size; ++i)
  {
    cout << tab[i];
    if(!((i+1)&31))
    {
      cout << endl;
    }
  }
  cout << endl;
}

int filesystem::cmp_inodes(inode a, inode b)
{
  return a.begin < b.begin;
}

void filesystem::download_file(string source)
{
  string dest = source; // plik na naszym dysku wirtualnym bedzie sie nazywac tak samo jak oryginal

  if(find_inode_by_name(dest) != NULL)
  {
    cout<<"Plik o takiej nazwie juz istnieje\n";
    return;
  }
  unsigned source_size;
  try
  {
    source_size = boost::filesystem::file_size(source);
  }
  catch(boost::filesystem::filesystem_error)
  {
    cout<<"Plik o takiej nazwie nie istnieje!\n";
    return;
  }
  unsigned source_blocks = (source_size - 1) / BLOCK_SIZE + 1; // zaokraglenie w gore
  unsigned total_blocks_free = _size - total_blocks_used;
  if(source_blocks > total_blocks_free)
  {
    cout<<"Brak miejsca na dysku";
    return;
  }
  total_blocks_used += source_blocks;
  unsigned position = alloc(source_blocks);
  sort(inodes.begin(), inodes.end(), cmp_inodes);
  buffer buffers[source_blocks];
  ifstream istr;
  istr.open(source.c_str());
  for(unsigned i=0; i < source_blocks; ++i)
  {
    istr.read(buffers[i], BLOCK_SIZE);
  }
  fstream ostr;
  ostr.open(_name.c_str(), ios::in|ios::out|ios::binary);
  ostr.seekp(position * BLOCK_SIZE);
  for(unsigned i=0; i < source_blocks; ++i)
  {
    ostr.write(buffers[i], BLOCK_SIZE);
  }
  inode new_inode;
  new_inode.used = 1;
  new_inode.begin = position;
  new_inode.size = source_size;
  new_inode.blocks = source_blocks;
  strcpy(new_inode.name, dest.c_str());
  inodes.push_back(new_inode);
}

void filesystem::upload_file(string dest_file)
{
  inode *node;
  if((node = find_inode_by_name(dest_file)) == NULL)
  {
    cout<<"Nie ma takiego pliku na dysku\n";
    return;
  }
  int counter = (int)node->size;
  buffer buf;
  ofstream dest;
  dest.open(dest_file.c_str());
  ifstream src;
  src.open(_name.c_str());
  src.seekg(node->begin * BLOCK_SIZE);
  unsigned curr_block_size = BLOCK_SIZE;
  while(counter > 0)
  {
    if(counter < (int)BLOCK_SIZE)
    {
      curr_block_size = counter;
    }
    src.read(buf, curr_block_size);
    dest.write(buf, curr_block_size);
    counter -= curr_block_size;
  }
  dest.close();
  src.close();
}

filesystem::inode* filesystem::find_inode_by_name(string source)
{
  for(unsigned i=0; i<inodes.size(); ++i)
  {
    if(inodes[i].name == source)
    {
      return &inodes[i];
    }
  }
  return NULL;
}

unsigned filesystem::alloc(unsigned blocks)
{
  int n=0;
  sort(inodes.begin(), inodes.end(), cmp_inodes);
  while(1)
  {
    unsigned nodes = inodes.size();
    if(nodes == 0 || inodes[0].begin - SYSTEM_BLOCKS >= blocks)
    // czy jest miejsce na poczatku?
    {
      return SYSTEM_BLOCKS;
    }
    for(unsigned i = 1; i < inodes.size(); ++i)
    {
      unsigned hole = inodes[i].begin - inodes[i-1].end();
      if(hole >= blocks)
      {
        return inodes[i-1].end();
      }
    }
    if(nodes != 0 && _size - inodes[nodes-1].end() >= blocks)
    {
      return inodes[nodes-1].end();
    }
    if(++n>1) throw std::runtime_error("Cos tu jest powaznie nie tak");
    // jesli nie udalo sie znalezc miejsca, to defragmentacja i jeszcze raz
    defragment();
  }
}

void filesystem::close()
{
  defragment();
  buffer buffers[SYSTEM_BLOCKS];
  unsigned index = 0;
  for(unsigned j=0; j < SYSTEM_BLOCKS; j++)
  {
    for(unsigned i=0; i < BLOCK_SIZE; i+=sizeof(inode), index++)
    {
      inode *ptri = reinterpret_cast<inode*>(buffers[j] + i);
      if(index < inodes.size())
      {
        *ptri = inodes[index];
      }
      else
      {
        //zeby sie mi jakies dziwne rzeczy tu nie dzialy
        for(unsigned k=0; k < sizeof(inode); ++k)
        {
          *(buffers[j] + i + k) = 0;
        }
      }
    }
  }
  fstream ostr;
  ostr.open(_name.c_str(), ios::in|ios::out|ios::binary);
  ostr.seekp(ios_base::beg);
  for(unsigned j=0; j < SYSTEM_BLOCKS; j++)
  {
    ostr.write(buffers[j], BLOCK_SIZE);
  }
}
