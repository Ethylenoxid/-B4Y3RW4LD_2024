#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>
using namespace std;

int read_net_statistics_file(string interface, string file_name){//reading out  /sys/class/net/{interface}/statistics/{file_name}
    int i=0;
    ifstream file_to_read;
    string file_content;
    string file_path = "/sys/class/net/"+interface+"/statistics/"+file_name;
    file_to_read.open(file_path);
    file_to_read >> file_content;
    istringstream(file_content)>>i;
    return i;
}




double get_average(double av, int i_value, int i_time){
    if (i_time==1){
        av=double(i_value);
    }
    else{
      av=(((double)i_time-1)*av+i_value)/((double)i_time);
    }
    return av;
}

void evaluate_file_data_alt(string interface){
    cout<<"Checking data for interface "<<interface<<endl;
    cout<<"time step"<<" "<<"tx_bytes"<<" "<<"rx_bytes"<<" "<<""<<endl;
    cout<<"time step"<<" "<<"tx_bytes"<<" "<<"rx_bytes"<<" "<<""<<endl;
    int i_time = 0;
    int i_tx, i_rx, max_tx, max_rx, min_tx, min_rx, av_tx, av_rx;
    max_tx = min_tx = av_tx = av_rx = 0;
    min_tx = min_rx = 473749806;
    while (true){
	i_time++;
	i_tx = read_net_statistics_file(interface, "tx_bytes");
	i_rx = read_net_statistics_file(interface, "rx_bytes");
	max_tx = max(max_tx, i_tx);
	max_rx = max(max_rx, i_rx);
	min_tx = min(min_tx, i_tx);
	min_rx = min(min_rx, i_rx);
	av_tx = get_average(av_tx, i_tx, i_time);
	av_rx = get_average(av_rx, i_rx, i_time);
        if (i_time%10==0){
            cout<<i_time<<" "<<i_tx<<" "<<i_rx<<" "<<av_tx<<" "<<min_tx<<" "<<max_tx<<endl;
        }
        else{
            cout<<i_time<<" "<<i_tx<<" "<<i_rx<<endl;
        }
    }
    this_thread::sleep_for(chrono::milliseconds(1000));
}

void evaluate_file_data(string interface){
    cout<<"Checking data for interface "<<interface<<endl;
    int i_time = 0;
    int i_btx = read_net_statistics_file(interface, "tx_bytes");
    int i_brx = read_net_statistics_file(interface, "rx_bytes");
    int i_ptx = read_net_statistics_file(interface, "tx_packets");
    int i_prx = read_net_statistics_file(interface, "rx_packets");
    int max_dbtx, min_dbtx, av_dbtx;
    max_dbtx = av_dbtx = 0;
    min_dbtx = 473749806;
    while (true){
	i_time++;
        int i_old_btx = i_btx;
	int i_old_brx = i_brx;
        int i_old_ptx = i_ptx;
	int i_old_prx = i_prx;
	i_btx = read_net_statistics_file(interface, "tx_bytes");
	i_brx = read_net_statistics_file(interface, "rx_bytes");
        i_ptx = read_net_statistics_file(interface, "tx_packets");
        i_prx = read_net_statistics_file(interface, "rx_packets");
	int d_btx = i_btx - i_old_btx;
	int d_brx = i_brx - i_old_brx;
	int d_ptx = i_ptx - i_old_ptx;
	int d_prx = i_prx - i_old_prx;
	min_dbtx = min(d_btx, min_dbtx);
	av_dbtx = get_average(av_dbtx, d_btx, i_time);
	max_dbtx = max (d_btx, max_dbtx);
        if (i_time%10==0){
            cout<<i_time<<" "<<d_btx<<" "<<d_brx<<" "<<d_ptx<<" "<<d_ptx<<" "<<min_dbtx<<" "<<av_dbtx<<" "<<max_dbtx<<endl;
        }
        else{
            cout<<i_time<<" "<<d_btx<<" "<<d_brx<<" "<<d_ptx<<" "<<d_ptx<<endl;
        }
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
    this_thread::sleep_for(chrono::milliseconds(40000));
}


int main(){    
    evaluate_file_data("wlo1");
    return 0;
}
