#include <iostream>
using namespace std;
/*Ban chat cua struc de luu duoc thong tin nhieu hon cho 1 bien */
struct PeoPle
{
    PeoPle(int age, float h, float w)
    {
        tuoi = age;
        chieucao = h;
        cannang = w;
    };
    int tuoi;
    float chieucao;
    float cannang;
};
int main()
{
    PeoPle Thao(22, 78.9, 56.6);
    PeoPle Lann(21, 67.8, 45.4);
    PeoPle Mai(20, 68.9, 67.6);
    //Tao ra 1 bien con tro khong co dinh 
    //Dau * nay la cau truc khai bao 
    PeoPle *bo_nhi = NULL;//chi coppy DL sang thui 
    //khi cap nhạt nó chả thay doi j 
    //Bo nhi khong pai la bien thuoc kieu du lieu con nguoi là  1 bien con trol 
    bo_nhi = &Thao;
    Thao.tuoi = 23;
    cout<< "Dia chi cua em Thao: " << &Thao <<endl;
    cout<< "Dia chi cua em Lan: " << &Lann <<endl;
    cout<< "Dia chi cua em Mai: " << &Mai <<endl;
    cout<< "Tuoi bo nhi: " << (*bo_nhi).tuoi <<endl;
    //(*bo_nhi) * o đay là toán tử . phép toán lấy giá trị tại địa chỉ 
    bo_nhi = &Lann;//luc nay bo_nhi la Lann r 
    cout<<"Vong 1 bo nhi: "<<(*bo_nhi).cannang <<endl;
    bo_nhi = bo_nhi + 1;
    cout<<"Vong 1 bo nhi: " <<(*bo_nhi).chieucao <<endl;
}