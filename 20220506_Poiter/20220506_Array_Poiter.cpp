#include<iostream>
#include<string.h>
using namespace std;
struct People
{
    People(string n, int a, float h, float w)
    {
        name = n;
        age  = a;
        heigh = h;
        weight = w;
        zin   = true;
    };
    string name;
    int age;
    float heigh, weight;
    bool zin;
};

void Pha(People *nguoi)
{
   cout<<"Thuc Hien Pha zin, em " << (*nguoi).name << endl;
   (*nguoi).zin = false;
}
int main()
{
    People Hoi_nguoi[5] =
    {
        People("Thao", 15, 56.7, 76.54),
        People("Nguy", 17, 65.6, 45.5),
        People("Hinh", 17, 65.6, 45.5),
        People("Ngoc", 17, 65.6, 45.5),
        People("Minh", 17, 65.6, 45.5),
    };
    //tao ra 1 con tro duyet cac phan tu cua mang 
    cout<< "Truoc khi pha, em " << Hoi_nguoi[3].name << " " << (Hoi_nguoi[3].zin ? "con_zin": "mat_zin")<< endl;
    Pha(&Hoi_nguoi[3]);
    cout<< "Sau khi pha, em " << Hoi_nguoi[3].name << " " << (Hoi_nguoi[3].zin ? "con_zin": "mat_zin")<< endl;
}