#include<iostream>
#include "filesystem.h"
using namespace std;

int main()
{
    string filename;
    char c;
    size_t size;
    filesystem *fs;
    while(true)
    {
        cout<<"Wybierz dzia�anie:\n"
            <<"1. Utworzenie nowego pliku dysku wirtualnego,\n"
            <<"2. Otwarcie istniej�cego pliku dysku wirtualnego,\n";
        cin>>c;
        if(c > '2' || c < '1')
        {
            cout<<"B��d! Komenda nie rozpoznana!\n";
            continue;
        }
        break;
    }
    cout<<"Podaj nazw� pliku dysku wirtualnego: ";
    cin>>filename;
    switch(c)
    {
    case '1':
        cout<<"Podaj wielko�� dysku wirtualnego (w kB): ";
        cin>>size;
        fs = new filesystem(filename, size);
        break;
    case '2':
        fs = new filesystem(filename);
        break;
    default:
        cout<<"Nieprzewidziany blad\n";
        return 0;
    }
    while(true)
    {
        cout<<"\nWybierz dzia�anie:\n"
            <<"1. Zamkni�cie wirtualnego dysku i utworzenie nowego,\n"
            <<"2. Kopiowanie pliku z dysku systemu na dysk wirtualny,\n"
            <<"3. Kopiowanie pliku z dysku wirtualnego na dysk systemu,\n"
            <<"4. Wy�wietlanie katalogu dysku wirtualnego,\n"
            <<"5. Usuwanie pliku z wirtualnego dysku,\n"
            <<"6. Wy�wietlenie zestawienia z aktualn� map� zaj�to�ci wirtualnego dysku,\n"
            <<"7. Defragmentacja wirtualnego dysku,\n"
            <<"8. Zamkni�cie wirtualnego dysku i zako�czenie programu\n";
        cin>>c;
        switch(c)
        {
        default:
            cout<<"B��d! Komenda nie rozpoznana!\n";
            break;
        case '1':
            delete fs;
            cout<<"Podaj nazw� dysku wirtualnego: ";
            cin>>filename;
            cout<<"Podaj wielko�� dysku wirtualnego (w kB): ";
            cin>>size;
            fs = new filesystem(filename, size);
            break;
        case '2':
            cout<<"Podaj nazw� pliku do skopiowania: ";
            cin>>filename;
            fs->download_file(filename);
            break;
        case '3':
            cout<<"Podaj nazw� pliku do skopiowania: ";
            cin>>filename;
            fs->upload_file(filename);
            break;
        case '4':
            fs->list_files();
            //cout<<"Aby kontynuowa�, naci�nij enter...\n";
            //getchar(c);
            break;
        case '5':
            cout<<"Podaj nazw� pliku do usuni�cia: ";
            cin>>filename;
            fs->remove_file(filename);
            break;
        case '6':
            fs->map();
            //cout<<"Aby kontynuowa�, naci�nij enter...\n";
            //getchar(c);
            break;
        case '7':
            fs->defragment();
            break;
        case '8':
            cout<<"Na pewno zamkn��? Y/N\n";
            cin>>c;
            if(c=='y'||c=='Y')
            {
                delete fs;
                return 0;
            }
            break;
        }
   }
}
