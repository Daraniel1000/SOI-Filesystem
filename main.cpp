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
        cout<<"Wybierz dzia³anie:\n"
            <<"1. Utworzenie nowego pliku dysku wirtualnego,\n"
            <<"2. Otwarcie istniej¹cego pliku dysku wirtualnego,\n";
        cin>>c;
        if(c > '2' || c < '1')
        {
            cout<<"B³¹d! Komenda nie rozpoznana!\n";
            continue;
        }
        break;
    }
    cout<<"Podaj nazwê pliku dysku wirtualnego: ";
    cin>>filename;
    switch(c)
    {
    case '1':
        cout<<"Podaj wielkoœæ dysku wirtualnego (w kB): ";
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
        cout<<"\nWybierz dzia³anie:\n"
            <<"1. Zamkniêcie wirtualnego dysku i utworzenie nowego,\n"
            <<"2. Kopiowanie pliku z dysku systemu na dysk wirtualny,\n"
            <<"3. Kopiowanie pliku z dysku wirtualnego na dysk systemu,\n"
            <<"4. Wyœwietlanie katalogu dysku wirtualnego,\n"
            <<"5. Usuwanie pliku z wirtualnego dysku,\n"
            <<"6. Wyœwietlenie zestawienia z aktualn¹ map¹ zajêtoœci wirtualnego dysku,\n"
            <<"7. Defragmentacja wirtualnego dysku,\n"
            <<"8. Zamkniêcie wirtualnego dysku i zakoñczenie programu\n";
        cin>>c;
        switch(c)
        {
        default:
            cout<<"B³¹d! Komenda nie rozpoznana!\n";
            break;
        case '1':
            delete fs;
            cout<<"Podaj nazwê dysku wirtualnego: ";
            cin>>filename;
            cout<<"Podaj wielkoœæ dysku wirtualnego (w kB): ";
            cin>>size;
            fs = new filesystem(filename, size);
            break;
        case '2':
            cout<<"Podaj nazwê pliku do skopiowania: ";
            cin>>filename;
            fs->download_file(filename);
            break;
        case '3':
            cout<<"Podaj nazwê pliku do skopiowania: ";
            cin>>filename;
            fs->upload_file(filename);
            break;
        case '4':
            fs->list_files();
            //cout<<"Aby kontynuowaæ, naciœnij enter...\n";
            //getchar(c);
            break;
        case '5':
            cout<<"Podaj nazwê pliku do usuniêcia: ";
            cin>>filename;
            fs->remove_file(filename);
            break;
        case '6':
            fs->map();
            //cout<<"Aby kontynuowaæ, naciœnij enter...\n";
            //getchar(c);
            break;
        case '7':
            fs->defragment();
            break;
        case '8':
            cout<<"Na pewno zamkn¹æ? Y/N\n";
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
