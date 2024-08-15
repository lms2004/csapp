
void explode_bomb(){};
//  phase_1
int main(){
    std::string string_ = read();
    bool check = strings_not_equal(string_,"对应参数");
    if (!check){
        explode_bomb();
    }
    return 0;
}


//  phase_2
int main(){
    int array[7];

    read_six_numbers(array);

    // 检查第一个数字是否为1
    if(array[0] == 1){
        explode_bomb();
    }

    // 检查剩余数字是否满足条件
    for(int i = 1; i <= 6; i++){
        if(array[i] != array[i-1] * 2){
            explode_bomb();
        }
    }

    return 0;
}
