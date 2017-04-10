/*                                                               Assignment 2  
    MEMBER 1 : 
        NAME : Chinmaya Pancholi
        ROLL NUMBER : 13CS30010

    MEMBER 2 : 
        NAME : Aishik Chakraborty
        ROLL NUMBER : 13CS30041
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>

#include <sys/socket.h> // For the socket (), bind () etc. functions.
#include <netinet/in.h> // For IPv4 data struct..
#include <string.h> // For memset.
#include <arpa/inet.h> // For inet_pton (), inet_ntop ().
#include <sys/select.h> // For select().
#include <bits/stdc++.h>
#include <errno.h>

using namespace std;

int port_num = 23465;
#define BUF_SIZE 100000

//class for booking details from the agent
class Booking_Details
{
	public:
		char passenger_id[100];
		char train[100];
		char coach_type[100];
		char berths[100];
		char preferences[100];
		char ages[100];
        char timestamp[100];
        int origin;
};

//class for Train details 
class Train_Details
{
    public : 
        int number; 
        char name[100];
        int ac_count;
        int ac_coach_su[50][10];
        int ac_coach_sl[50][10];
        int ac_coach_ub[50][19];
        int ac_coach_mb[50][19];
        int ac_coach_lb[50][19];

        int sleeper_count;
        int sleeper_coach_su[50][10];
        int sleeper_coach_sl[50][10];
        int sleeper_coach_ub[50][19];
        int sleeper_coach_mb[50][19];
        int sleeper_coach_lb[50][19];

        int su;
        int sl;
        int ub;
        int mb;
        int lb;
};

//comparator function for sorting the elements of the vector containing orders from agents
int comp_func1(const Booking_Details &o1, const Booking_Details &o2)
{
    if(atoi(o1.berths) > atoi(o2.berths)) return 1;
    else if(atoi(o1.berths) < atoi(o2.berths)) return 0;
    else if(atoi(o1.berths) == atoi(o2.berths))
    {
        char *indiv_token;
        int o1_count=0;
        int j=0;
        int curr_num=0;
        while(1)
        {
            if(j==strlen(o1.ages))
            {
                if(curr_num>60)
                {
                    o1_count++;
                }
                break;
            }
            else if(o1.ages[j]!='-')
            {
                curr_num = 10*curr_num + (o1.ages[j]-'0');
            }
            else if(o1.ages[j]=='-')
            {
                if(curr_num>60)
                {
                    o1_count++;
                }
                curr_num=0;
            }
        }

        j=0;
        int o2_count=0;
        while(1)
        {
            if(j==strlen(o2.ages))
            {
                if(curr_num>60)
                {
                    o2_count++;
                }
                break;
            }
            else if(o2.ages[j]!='-')
            {
                curr_num = 10*curr_num + (o2.ages[j]-'0');
            }
            else if(o2.ages[j]=='-')
            {
                if(curr_num>60)
                {
                    o2_count++;
                }
                curr_num=0;
            }
        }

        /*while((indiv_token = strsep(&(o1.ages), "-")) != NULL)
        {
          if(strcmp(indiv_token, "") != 0)
          {
            if(atoi(indiv_token) > 60)
            {
                o1_count++;
            } 
          }
        }

        int o2_count=0;
        while((indiv_token = strsep(&(o2.ages), "-")) != NULL)
        {
          if(strcmp(indiv_token, "") != 0)
          {
            if(atoi(indiv_token) > 60)
            {
                o2_count++;
            } 
          }
        }*/

        if(o1_count > o2_count) return 1;
        else if(o1_count < o2_count) return 0;
        else if(o1_count == o2_count)
        {
            long long int o1_timestamp=0 ;    //= atoi(o1.timestamp);
            long long int o2_timestamp=0 ;    //= atoi(o2.timestamp);
            int j;
            for(j=0;j<strlen(o1.timestamp);j++)
            {
                o1_timestamp = 10*o1_timestamp + (o1.timestamp[j]-'0');

            }

            for(j=0;j<strlen(o2.timestamp);j++)
            {
                o2_timestamp = 10*o2_timestamp + (o2.timestamp[j]-'0');

            }

            if(o1_timestamp >= o2_timestamp) return 1;
            else if(o1_timestamp < o2_timestamp) return 0;
        }
    }
}

//function to count occurrences of s2 in s1
int count_occurences(char s1[1000], char s2[1000])
{
  char *p;
  size_t count = 0;
  size_t len1;

  p = strchr(s1, '\n');
  if (p) *p = 0;

  p = strchr(s2, '\n');            
  if (p) *p = 0;

  p = s1;
  len1 = strlen(s2);

  while ((p = strstr(p, s2)) != NULL && p != s2)
  {
    count++;
    p += len1;
  }
  return count;
}


//function to check if there exists a single coach for all the passangers
int is_suitable(Train_Details train_1, int su_req, int sl_req, int ub_req, int mb_req, int lb_req, int type)
{
    int i,j;
    int su_left = su_req;
    int sl_left = sl_req;
    int ub_left = ub_req;
    int mb_left = mb_req;
    int lb_left = lb_req;

    if(type == 'a')
    {
        for(i=1;i<=train_1.ac_count;i++)
        {
            su_left = su_req;
            sl_left = sl_req;
            ub_left = ub_req;
            mb_left = mb_req;
            lb_left = lb_req;

            for(j=1;j<=9 && su_left>0 && (train_1.su==1);j++)
            {
                if(train_1.ac_coach_su[i][j]==0)
                {
                    su_left--;
                }
            }

            for(j=1;j<=9 && sl_left>0  && (train_1.sl==1);j++)
            {
                if(train_1.ac_coach_sl[i][j]==0)
                {
                    sl_left--;
                }
            }

            for(j=1;j<=9 && ub_left>0  && (train_1.ub==1);j++)
            {
                if(train_1.ac_coach_ub[i][j]==0)
                {
                    ub_left--;
                }
            }

            for(j=1;j<=9 && mb_left>0  && (train_1.mb==1);j++)
            {
                if(train_1.ac_coach_mb[i][j]==0)
                {
                    mb_left--;
                }
            }

            for(j=1;j<=9 && lb_left>0  && (train_1.lb==1);j++)
            {
                if(train_1.ac_coach_lb[i][j]==0)
                {
                    lb_left--;
                }
            }
            if(su_left == 0 && sl_left == 0 && ub_left == 0 && mb_left == 0 && lb_left == 0)
            {
                return i;
            }
        }
    }
    else if(type =='s')
    {
        for(i=1;i<=train_1.sleeper_count;i++)
        {
            su_left = su_req;
            sl_left = sl_req;
            ub_left = ub_req;
            mb_left = mb_req;
            lb_left = lb_req;

            for(j=1;j<=9 && su_left>0 && (train_1.su==1);j++)
            {
                if(train_1.sleeper_coach_su[i][j]==0)
                {
                    su_left--;
                }
            }

            for(j=1;j<=9 && sl_left>0  && (train_1.sl==1);j++)
            {
                if(train_1.sleeper_coach_sl[i][j]==0)
                {
                    sl_left--;
                }
            }


            for(j=1;j<=9 && ub_left>0  && (train_1.ub==1);j++)
            {
                if(train_1.sleeper_coach_ub[i][j]==0)
                {
                    ub_left--;
                }
            }

            for(j=1;j<=9 && mb_left>0  && (train_1.mb==1);j++)
            {
                if(train_1.sleeper_coach_mb[i][j]==0)
                {
                    mb_left--;
                }
            }

            for(j=1;j<=9 && lb_left>0  && (train_1.lb==1);j++)
            {
                if(train_1.sleeper_coach_lb[i][j]==0)
                {
                    lb_left--;
                }
            }
            if(su_left == 0 && sl_left == 0 && ub_left == 0 && mb_left == 0 && lb_left == 0)
            {
                return i;
            }            
        }
    }
    return (-1);
}


//is_possible(train1, atoi(req_list[i].berths), 'a')
//function to check if there exists any coach for all the passangers' allotment
int is_possible(Train_Details train_1, int req, char type)
{
    int i,j;
    int left = req;
    if(type=='a')
    {       
        //cout<<"JJJJJ1 Here\n";
        for(i=1;i<=train_1.ac_count;i++)
        {
            for(j=1;j<=9 && left>0 && (train_1.su ==1) ;j++)
            {
                if(train_1.ac_coach_su[i][j]==0)
                {
                    left--;
                }
            }

            for(j=1;j<=9 && left>0 && (train_1.sl ==1) ;j++)
            {
                if(train_1.ac_coach_sl[i][j]==0)
                {
                    left--;
                }
            }

            for(j=1;j<=9 && left>0 && (train_1.ub ==1) ;j++)
            {
                if(train_1.ac_coach_ub[i][j]==0)
                {
                    left--;
                }
            }

            for(j=1;j<=9 && left>0 && (train_1.mb ==1) ;j++)
            {
                if(train_1.ac_coach_mb[i][j]==0)
                {
                    left--;
                }
            }

            for(j=1;j<=9 && left>0 && (train_1.lb ==1) ;j++)
            {
                if(train_1.ac_coach_lb[i][j]==0)
                {
                    left--;
                }
            }
        }


        if(left==0) 
        {
            //cout<<" returning 1\n";
            return 1;
        }
        else 
        {
            return 0;       
            //cout<<" returning 0\n";
        }
    }
    else if(type =='s')
    {
        for(i=1;i<=train_1.sleeper_count;i++)
        {
            for(j=1;j<=9 && left>0 && (train_1.su ==1) ;j++)
            {
                if(train_1.sleeper_coach_su[i][j]==0)
                {
                    left--;
                }
            }

            for(j=1;j<=9 && left>0 && (train_1.sl ==1) ;j++)
            {
                if(train_1.sleeper_coach_sl[i][j]==0)
                {
                    left--;
                }
            }

            for(j=1;j<=9 && left>0 && (train_1.ub ==1) ;j++)
            {
                if(train_1.sleeper_coach_ub[i][j]==0)
                {
                    left--;
                }
            }

            for(j=1;j<=9 && left>0 && (train_1.mb ==1) ;j++)
            {
                if(train_1.sleeper_coach_mb[i][j]==0)
                {
                    left--;
                }
            }

            for(j=1;j<=9 && left>0 && (train_1.lb ==1) ;j++)
            {
                if(train_1.sleeper_coach_lb[i][j]==0)
                {
                    left--;
                }
            }
        }

        if(left==0) return 1;
        else return 0;
    }
}


//best_poss_coach_na(train1, 'a')
//function to get the best possible coach for the passangers with NA preference
int best_poss_coach_na(Train_Details train_1, char type)
{
    int max_till_now = -1;
    int res = -1;
    int count=0;
    int i,j;

    if(type=='a')
    {       
        //cout<<"In func1\n";

        for(i=1;i<=train_1.ac_count;i++)
        {
            count=0;
            for(j=1;j<=9 && (train_1.su ==1) ;j++)
            {
                if(train_1.ac_coach_su[i][j]==0)
                {
                    count++;
                }
            }

            for(j=1;j<=9 && (train_1.sl ==1) ;j++)
            {
                if(train_1.ac_coach_sl[i][j]==0)
                {
                    count++;
                }
            }

            for(j=1;j<=9 && (train_1.ub ==1) ;j++)
            {
                if(train_1.ac_coach_ub[i][j]==0)
                {
                    count++;
                }
            }

            for(j=1;j<=9 && (train_1.mb ==1) ;j++)
            {
                if(train_1.ac_coach_mb[i][j]==0)
                {
                    count++;
                }
            }

            for(j=1;j<=9 && (train_1.lb ==1) ;j++)
            {
                if(train_1.ac_coach_lb[i][j]==0)
                {
                    count++;
                }
            }

            if(count > max_till_now)
            {
                max_till_now = count;
                res = i;
                //cout<<"count -> "<<count<<"\t"<<"Res -> "<<res<<"\n";
            }
        }
        return res;
    }
    else if(type =='s')
    {
        for(i=1;i<=train_1.sleeper_count;i++)
        {
            count=0;
            for(j=1;j<=9 && (train_1.su ==1) ;j++)
            {
                if(train_1.sleeper_coach_su[i][j]==0)
                {
                    count++;
                }
            }

            for(j=1;j<=9 && (train_1.sl ==1) ;j++)
            {
                if(train_1.sleeper_coach_sl[i][j]==0)
                {
                    count++;
                }
            }

            for(j=1;j<=9 && (train_1.ub ==1) ;j++)
            {
                if(train_1.sleeper_coach_ub[i][j]==0)
                {
                    count++;
                }
            }

            for(j=1;j<=9 && (train_1.mb ==1) ;j++)
            {
                if(train_1.sleeper_coach_mb[i][j]==0)
                {
                    count++;
                }
            }

            for(j=1;j<=9 && (train_1.lb ==1) ;j++)
            {
                if(train_1.sleeper_coach_lb[i][j]==0)
                {
                    count++;
                }
            }
            if(count>max_till_now)
            {
                max_till_now = count;
                res = i;
            }
        } 
        return res;
    }   
}


//int best_poss_coach(int arr_su[50][10], int arr_sl[50][10], int arr_ub[50][19], int arr_mb[50][19], int arr_lb[50][19], int su_req, int sl_req, int ub_req, int mb_req, int lb_req, int coach_count)
//function to get the best possible coach for the passangers
int best_poss_coach(Train_Details train_1, int su_req, int sl_req, int ub_req, int mb_req, int lb_req, char type)
{
    int max_till_now = -1;
    int res = -1;
    int i,j;
    int count;
    int su_left = su_req;
    int sl_left = sl_req;
    int ub_left = ub_req;
    int mb_left = mb_req;
    int lb_left = lb_req;

    if(type=='a')
    {
        for(i=1;i<=train_1.ac_count;i++)
        {
            su_left = su_req;
            sl_left = sl_req;
            ub_left = ub_req;
            mb_left = mb_req;
            lb_left = lb_req;

            count=0;

            for(j=1;j<=9 && (train_1.su ==1) && su_left>0 ;j++)
            {
                if(train_1.ac_coach_su[i][j]==0)
                {
                    count++;
                    su_left--;
                }
            }

            for(j=1;j<=9 && (train_1.sl ==1) && sl_left>0 ;j++)
            {
                if(train_1.ac_coach_sl[i][j]==0)
                {
                    count++;
                    sl_left--;
                }
            }

            for(j=1;j<=9 && (train_1.ub ==1) && ub_left>0 ;j++)
            {
                if(train_1.ac_coach_ub[i][j]==0)
                {
                    count++;
                    ub_left--;
                }
            }

            for(j=1;j<=9 && (train_1.mb ==1) && mb_left>0 ;j++)
            {
                if(train_1.ac_coach_mb[i][j]==0)
                {
                    count++;
                    mb_left--;
                }
            }

            for(j=1;j<=9 && (train_1.lb ==1) && lb_left>0 ;j++)
            {
                if(train_1.ac_coach_lb[i][j]==0)
                {
                    count++;
                    lb_left--;
                }
            }
            if(count > max_till_now)
            {
                max_till_now = count;
                res = i;
            }
        } 
        return res;
    }
    else if(type=='s')
    {
        for(i=1;i<=train_1.sleeper_count;i++)
        {
            su_left = su_req;
            sl_left = sl_req;
            ub_left = ub_req;
            mb_left = mb_req;
            lb_left = lb_req;

            count=0;

            for(j=1;j<=9 && (train_1.su ==1) && su_left>0 ;j++)
            {
                if(train_1.sleeper_coach_su[i][j]==0)
                {
                    count++;
                    su_left--;
                }
            }

            for(j=1;j<=9 && (train_1.sl ==1) && sl_left>0 ;j++)
            {
                if(train_1.sleeper_coach_sl[i][j]==0)
                {
                    count++;
                    sl_left--;
                }
            }

            for(j=1;j<=9 && (train_1.ub ==1) && ub_left>0 ;j++)
            {
                if(train_1.sleeper_coach_ub[i][j]==0)
                {
                    count++;
                    ub_left--;
                }
            }

            for(j=1;j<=9 && (train_1.mb ==1) && mb_left>0 ;j++)
            {
                if(train_1.sleeper_coach_mb[i][j]==0)
                {
                    count++;
                    mb_left--;
                }
            }

            for(j=1;j<=9 && (train_1.lb ==1) && lb_left>0 ;j++)
            {
                if(train_1.sleeper_coach_lb[i][j]==0)
                {
                    count++;
                    lb_left--;
                }
            }
            if(count > max_till_now)
            {
                max_till_now = count;
                res = i;
            }
        } 
        return res;
    }
}

//function to get the status of the trains
void show_status(Train_Details t1, Train_Details t2)
{
    int t1_total_ac = 216;
    int t1_total_sleeper = 864;
    int t1_ac_available = 0;
    int t1_sleeper_available = 0;

    int t2_total_ac = 702;
    int t2_total_sleeper = 0;
    int t2_ac_available = 0;
    int t2_sleeper_available = 0;

    int i,j;

                        for(i=1;i<=t1.ac_count;i++)
                        {
                            for(j=1;j<=9 && (t1.su ==1) ;j++)
                            {
                                    if(t1.ac_coach_su[i][j]==0)
                                    {
                                        t1_ac_available++;
                                    }
                            }

                            for(j=1;j<=9 && (t1.sl ==1) ;j++)
                            {
                                    if(t1.ac_coach_sl[i][j]==0)
                                    {
                                        t1_ac_available++;
                                    }
                            }

                            for(j=1;j<=18 && (t1.ub ==1) ;j++)
                            {
                                    if(t1.ac_coach_ub[i][j]==0)
                                    {
                                        t1_ac_available++;
                                    }
                            }

                            for(j=1;j<=18 && (t1.mb ==1) ;j++)
                            {
                                    if(t1.ac_coach_mb[i][j]==0)
                                    {
                                        t1_ac_available++;
                                    }
                            }

                            for(j=1;j<=18 && (t1.lb ==1) ;j++)
                            {
                                    if(t1.ac_coach_lb[i][j]==0)
                                    {
                                        t1_ac_available++;
                                    }
                            }                            
                        }

                        for(i=1;i<=t1.sleeper_count;i++)
                        {
                            for(j=1;j<=9 && (t1.su ==1) ;j++)
                            {
                                    if(t1.sleeper_coach_su[i][j]==0)
                                    {
                                        t1_sleeper_available++;
                                    }
                            }

                            for(j=1;j<=9 && (t1.sl ==1) ;j++)
                            {
                                    if(t1.sleeper_coach_sl[i][j]==0)
                                    {
                                        t1_sleeper_available++;
                                    }
                            }

                            for(j=1;j<=18 && (t1.ub ==1) ;j++)
                            {
                                    if(t1.sleeper_coach_ub[i][j]==0)
                                    {
                                        t1_sleeper_available++;
                                    }
                            }

                            for(j=1;j<=18 && (t1.mb ==1) ;j++)
                            {
                                    if(t1.sleeper_coach_mb[i][j]==0)
                                    {
                                        t1_sleeper_available++;
                                    }
                            }

                            for(j=1;j<=18 && (t1.lb ==1) ;j++)
                            {
                                    if(t1.sleeper_coach_lb[i][j]==0)
                                    {
                                        t1_sleeper_available++;
                                    }
                            }                            
                        }


                        for(i=1;i<=t2.ac_count;i++)
                        {
                            for(j=1;j<=9 && (t2.su ==1) ;j++)
                            {
                                    if(t2.ac_coach_su[i][j]==0)
                                    {
                                        t2_ac_available++;
                                    }
                            }

                            for(j=1;j<=9 && (t2.sl ==1) ;j++)
                            {
                                    if(t2.ac_coach_sl[i][j]==0)
                                    {
                                        t2_ac_available++;
                                    }
                            }

                            for(j=1;j<=18 && (t2.ub ==1) ;j++)
                            {
                                    if(t2.ac_coach_ub[i][j]==0)
                                    {
                                        t2_ac_available++;
                                    }
                            }

                            /*for(j=1;j<=18 && (t2.mb ==1) ;j++)
                            {
                                    if(t2.ac_coach_mb[i][j]==0)
                                    {
                                        t2_ac_available++;
                                    }
                            }*/

                            for(j=1;j<=18 && (t2.lb ==1) ;j++)
                            {
                                    if(t2.ac_coach_lb[i][j]==0)
                                    {
                                        t2_ac_available++;
                                    }
                            }                            
                        }

                        for(i=1;i<=t2.sleeper_count;i++)
                        {
                            for(j=1;j<=9 && (t2.su ==1) ;j++)
                            {
                                    if(t2.sleeper_coach_su[i][j]==0)
                                    {
                                        t2_sleeper_available++;
                                    }
                            }

                            for(j=1;j<=9 && (t2.sl ==1) ;j++)
                            {
                                    if(t2.sleeper_coach_sl[i][j]==0)
                                    {
                                        t2_sleeper_available++;
                                    }
                            }

                            for(j=1;j<=18 && (t2.ub ==1) ;j++)
                            {
                                    if(t2.sleeper_coach_ub[i][j]==0)
                                    {
                                        t2_sleeper_available++;
                                    }
                            }

                            /*for(j=1;j<=18 && (t2.mb ==1) ;j++)
                            {
                                    if(t2.sleeper_coach_mb[i][j]==0)
                                    {
                                        t2_sleeper_available++;
                                    }
                            }*/

                            for(j=1;j<=18 && (t2.lb ==1) ;j++)
                            {
                                    if(t2.sleeper_coach_lb[i][j]==0)
                                    {
                                        t2_sleeper_available++;
                                    }
                            }                            
                        }
        cout<<"TRAIN\t\tAC TOTAL[BOOKED/AVAILABLE]\t\tSLEEPER TOTAL[BOOKED/AVAILABLE]\n\n";
        cout<<"\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
        cout<<t1.name<<"\t\t"<<t1_total_ac<<"["<<(t1_total_ac-t1_ac_available)<<"/"<<t1_ac_available<<"]\t\t"<<t1_total_sleeper<<"["<<(t1_total_sleeper-t1_sleeper_available)<<"/"<<(t1_sleeper_available)<<"]\n\n";
        cout<<"\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
        cout<<t2.name<<"\t\t"<<t2_total_ac<<"["<<(t2_total_ac-t2_ac_available)<<"/"<<t2_ac_available<<"]\t\t"<<t2_total_sleeper<<"["<<(t2_total_sleeper-t2_sleeper_available)<<"/"<<(t2_sleeper_available)<<"]\n\n";
        cout<<"\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
        cout<<"\n\n";
}


int main(int argc, char const *argv[])
{
    Train_Details train1;
    train1.number=12321;
    strcpy(train1.name,"Superfast Exp");
    train1.ac_count=3;
    //train1.ac_seats_per_coach=72;
    train1.sleeper_count=12;
    //train1.sleeper_seats_per_coach=72;
    train1.su=1;
    train1.sl=1;
    train1.ub=1;
    train1.mb=1;
    train1.lb=1;

    for(int i=1;i<=train1.ac_count;i++)
    {
        if(train1.su==1) memset(train1.ac_coach_su,0,sizeof(train1.sleeper_coach_su));
        if(train1.sl==1) memset(train1.ac_coach_sl,0,sizeof(train1.sleeper_coach_sl));
        if(train1.ub==1) memset(train1.ac_coach_ub,0,sizeof(train1.sleeper_coach_ub));
        if(train1.mb==1) memset(train1.ac_coach_mb,0,sizeof(train1.sleeper_coach_mb));
        if(train1.lb==1) memset(train1.ac_coach_lb,0,sizeof(train1.sleeper_coach_lb));
    }

    for(int i=1;i<=train1.sleeper_count;i++)
    {
        if(train1.su==1) memset(train1.sleeper_coach_su,0,sizeof(train1.sleeper_coach_su));
        if(train1.sl==1) memset(train1.sleeper_coach_sl,0,sizeof(train1.sleeper_coach_sl));
        if(train1.ub==1) memset(train1.sleeper_coach_ub,0,sizeof(train1.sleeper_coach_ub));
        if(train1.mb==1) memset(train1.sleeper_coach_mb,0,sizeof(train1.sleeper_coach_mb));
        if(train1.lb==1) memset(train1.sleeper_coach_lb,0,sizeof(train1.sleeper_coach_lb));
    }


    Train_Details train2;
    train2.number=12301;
    strcpy(train2.name,"Rajdhani Exp");
    train2.ac_count=13;
    //train2.ac_seats_per_coach=54;
    train2.sleeper_count=0;
    //train2.sleeper_seats_per_coach=0;
    train2.su=1;
    train2.sl=1;
    train2.ub=1;
    train2.mb=0;
    train2.lb=1;

    for(int i=1;i<=train2.ac_count;i++)
    {
        if(train2.su==1) memset(train2.ac_coach_su,0,sizeof(train2.sleeper_coach_su));
        if(train2.sl==1) memset(train2.ac_coach_sl,0,sizeof(train2.sleeper_coach_sl));
        if(train2.ub==1) memset(train2.ac_coach_ub,0,sizeof(train2.sleeper_coach_ub));
        if(train2.mb==1) memset(train2.ac_coach_mb,0,sizeof(train2.sleeper_coach_mb));
        if(train2.lb==1) memset(train2.ac_coach_lb,0,sizeof(train2.sleeper_coach_lb));
    }

    for(int i=1;i<=train2.sleeper_count;i++)
    {
        if(train2.su==1) memset(train2.sleeper_coach_su,0,sizeof(train2.sleeper_coach_su));
        if(train2.sl==1) memset(train2.sleeper_coach_sl,0,sizeof(train2.sleeper_coach_sl));
        if(train2.ub==1) memset(train2.sleeper_coach_ub,0,sizeof(train2.sleeper_coach_ub));
        if(train2.mb==1) memset(train2.sleeper_coach_mb,0,sizeof(train2.sleeper_coach_mb));
        if(train2.lb==1) memset(train2.sleeper_coach_lb,0,sizeof(train2.sleeper_coach_lb));
    }


	int l = 100;
    int j;
	int rst; // Return status of functions.
    int cfd[100] = {0}; // File descriptor for the clients.

    /**************** Create a socket. *******************************/
    int sfd; // Socket file descriptor.
    sfd = socket (AF_INET, SOCK_STREAM, 0); /* AF_INET --> IPv4,
                * SOCK_STREAM --> TCP Protocol, 0 --> for the protocol. */
    if (sfd == -1) 
    {
        perror ("Server: socket error");
        exit (1);
    }
    printf ("Socket fd = %d\n", sfd);

    int iSetOption=1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,sizeof(iSetOption));    
    
    
    /***************** Assign an address to the server **************/
    struct sockaddr_in srv_addr, cli_addr; // Addresses of the server and the client.
    socklen_t addrlen = sizeof (struct sockaddr_in); // size of the addresses.
    
    // Clear the two addresses.
    memset (&srv_addr, 0, addrlen);
    memset (&cli_addr, 0, addrlen);

    // Assign values to the server address.
    srv_addr.sin_family = AF_INET; // IPv4.
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    srv_addr.sin_port   = htons (port_num); // Port Number.
    
    /*rst = inet_pton (AF_INET, "127.0.0.1", &srv_addr.sin_addr); 

    if (rst <= 0)
    {
        perror ("Server Presentation to network address conversion.\n");
        exit (1);
    } */   
    
    
    
    /****************** Bind the server to an address. ***************/
    rst = bind (sfd, (struct sockaddr *) &srv_addr, addrlen); /* Note
        * the type casting of the pointer to the server's address. */
    if (rst == -1)
    {
        perror ("Server: Bind failed");
        exit (1);
    }
    
    
    
    /***************** listen (): Mark the socket as passive. *******/
    printf ("Max connections allowed to wait: %d\n", SOMAXCONN);
    rst = listen (sfd, 1);
    if (rst == -1)
    {
        perror ("Server: Listen failed");
        exit (1);
    }

    int i = 0;

    fd_set rd;
    FD_SET(0,&rd);


//    fd_set rd;
//   FD_SET(0, &rd);

	while(1)
	{
        struct timeval s1;
        s1.tv_sec=2;
        s1.tv_usec=0;

        struct timeval s2;
        s2.tv_sec=2;
        s2.tv_usec=0;

        sleep(3);
        //cout<<"+++++Starting1\n";

        FD_SET(0, &rd);
        int rd_ret = select(100, &rd, NULL,NULL, &s1);

        if(FD_ISSET(0,&rd))
        {
            //printf("heretttttt\n");
            int fork_ret1 = fork();
            if(fork_ret1==0)
            {
                int n;
                scanf("%d",&n);
                if(n==1)
                {
                    //printf("here1111111\n");
                    show_status(train1, train2);
                }
                exit(0);
            }
        }

        //result = select(sfd + 1, &read, NULL, NULL, NULL);
        
		Booking_Details ob1;
		vector<Booking_Details> v;

		fd_set read;
		FD_ZERO(&read);
   		FD_SET(sfd, &read);
        //FD_SET(0, &read);

   		int max_fd = sfd;

   		int sd;
   		//result = select(sfd + 1, &read, NULL, NULL, NULL);

		/***************** accept (): Waiting for connections ************/
		for(i = 0; i < l; i++)
		{
			sd = cfd[i];
			if(sd > 0)
				FD_SET(sd ,&read);
			if(sd > max_fd)
				max_fd = sd;
		}


		int scan_activity;
		scan_activity = select(max_fd+1, &read, NULL, NULL, &s2);

		if ((scan_activity < 0) && (errno!=EINTR)) 
        {
            printf("select error");
        }


        if(FD_ISSET(sfd, &read))
        {
        	int new_socket;
        	if((new_socket = accept (sfd, (struct sockaddr *) &cli_addr, &addrlen))<0)
        	{
        		perror("Error Accept");
        		exit(EXIT_FAILURE);
        	}
            cout<<"New socket = > "<<new_socket<<"\n";
        	for(i = 0; i < l; i++)
        	{
	        	if(cfd[i] == 0)
	        	{
	        		cfd[i] = new_socket;
	        		break;
	        	}
        	}
        }
        /*else 
        {
            continue;
            cout<<"Else 1\n";
        }*/

        int bytesReceived;
        char *msg_recv;

        vector<Booking_Details> req_list;
        req_list.clear();

        char buff[BUF_SIZE];
        for (i = 0; i < l; i++) 
        {
            sd = cfd[i];
            if(sd==0) continue;
            bzero(buff, BUF_SIZE);
                
            if (FD_ISSET(sd , &read)) 
            {
            	if((bytesReceived = recv(cfd[i], buff, BUF_SIZE, 0)) > 0)
                {
                    msg_recv = strdup(buff);
                    cout<<"received -> " <<msg_recv<<"\n";
                    Booking_Details o1;
                    int j=0;
                    int k=0;
                    /*
                            char passenger_id[100];
                            char train[100];
                            char coach_type[100];
                            char berths;
                            char preferences[100];
                            char ages[100];
                            char timestamp[100];
                    */
                    //cout<<"Here11";
                    while(j<strlen(msg_recv) && msg_recv[j]!=',')
                    {
                        o1.passenger_id[k++] = msg_recv[j++];
                    }
                    o1.passenger_id[k]='\0';
                    j++;

                    k=0;
                    while(j<strlen(msg_recv) && msg_recv[j]!=',')
                    {
                        o1.train[k++] = msg_recv[j++];
                    }
                    o1.train[k]='\0';
                    j++;

                    k=0;
                    while(j<strlen(msg_recv) && msg_recv[j]!=',')
                    {
                        o1.coach_type[k++] = msg_recv[j++];
                    }
                    o1.coach_type[k]='\0';
                    j++;

                    k=0;
                    while(j<strlen(msg_recv) && msg_recv[j]!=',')
                    {
                        o1.berths[k++] = msg_recv[j++];
                    }
                    o1.berths[k]='\0';
                    j++;

                    k=0;
                    while(j<strlen(msg_recv) && msg_recv[j]!=',')
                    {
                        o1.preferences[k++] = msg_recv[j++];
                    }
                    o1.preferences[k]='\0';
                    j++;

                    k=0;
                    while(j<strlen(msg_recv) && msg_recv[j]!=',')
                    {
                        o1.ages[k++] = msg_recv[j++];
                    }
                    o1.ages[k]='\0';
                    j++;

                    k=0;
                    while(j<strlen(msg_recv) && msg_recv[j]!=',')
                    {
                        o1.timestamp[k++] = msg_recv[j++];
                    }
                    o1.timestamp[k]='\0';


                    o1.origin=sd;
                        
                    //cout<<"SD - > "<<sd<<"\n";
                    //cout<<"O1.ORIGIN - > "<<o1.origin<<"\n";
                        
                    //cout<<"Here22";

                    cout<<"Req details : \n";
                    cout<<o1.passenger_id<<"\t"<<o1.train<<"\t"<<o1.coach_type<<"\t"<<o1.berths<<"\t"<<o1.preferences<<"\t"<<o1.ages<<"\t"<<o1.timestamp<<"\t"<<o1.origin<<"\n";

                    req_list.push_back(o1);
                    bzero(buff, BUF_SIZE);

                            /*char passenger_id[100];
                            char train[100];
                            char coach_type[100];
                            char berths[100];
                            char preferences[100];
                            char ages[100];
                            char timestamp[100];*/
                }
                else 
                {
                    close(cfd[i]);
                    cfd[i]=0;
                    cout<<"Nothing Received.\n";
                }
            }      
        }

        //if(req_list.size()!=0) cout<<"List size1 - > "<<req_list.size()<<"\n";
        //else cout<<"SIZE1 -> 0";

        sort(req_list.begin(), req_list.end(), comp_func1);         //sorting the incoming requests
        //cout<<"Here33\n";

        int su_req, sl_req, ub_req, mb_req, lb_req;


        //if(req_list.size()!=0) cout<<"List size1 - > "<<req_list.size()<<"\n";
        //else cout<<"SIZE1 - > 0";
        
        char response_to_agent[BUF_SIZE];

        int k;
        
        for(i=0;i<req_list.size();i++)  //dealing with each request
        {
            k = i;
            bzero(response_to_agent, BUF_SIZE);
            int origin_of_msg = req_list[i].origin;
            cout<<"rl[i].ORIGIN - > "<<req_list[i].origin<<"\n";

            strcat(response_to_agent, req_list[i].passenger_id);
            if(strcmp(req_list[i].train,"12321")==0 && strcmp(req_list[i].coach_type,"AC")==0)
            {
                //cout<<"Here44\n";
                if(is_possible(train1, atoi(req_list[i].berths), 'a')==0)
                {
                    //printf("NP1\n");
                    strcat(response_to_agent, ",Sorry! no berths available!\n");

                    cout<<"Generated resp - > "<<response_to_agent;

                    int rst = send (origin_of_msg, response_to_agent, strlen(response_to_agent), 0);
                    if (rst == -1)
                    {
                        //cerr<<"Hello1\n";
                        perror ("4. Server: Send failed");
                    }
                    else 
                    {
                        //cerr<<"rst -> "<<rst<<"\n";
                        printf ("Server 2 Client message sent successfully.\n");
                        fflush(stdout);
                    }

                    continue;

                    //goto end_part;
                    //continue;                    
                }
                //else cout<<"P1\n";
            }
            else if(strcmp(req_list[i].train,"12321")==0 && strcmp(req_list[i].coach_type,"Sleeper")==0)
            {
                //cout<<"Here case11\n";  
                if(is_possible(train1, atoi(req_list[i].berths), 's')==0)
                {
                    strcat(response_to_agent, ",Sorry! no berths available!\n");

                    cout<<"Generated resp - > "<<response_to_agent;

                    int rst = send (origin_of_msg, response_to_agent, strlen(response_to_agent), 0);
                    if (rst == -1)
                    {
                        //cerr<<"Hello1\n";
                        perror ("4. Server: Send failed");
                    }
                    else 
                    {
                        //cerr<<"rst -> "<<rst<<"\n";
                        printf ("Server 2 Client message sent successfully.\n");
                        fflush(stdout);
                    }
                    continue;

                    //goto end_part;
                    //continue;                    
                }
            }
            else if(strcmp(req_list[i].train,"12301")==0 && strcmp(req_list[i].coach_type,"AC")==0)
            {
                if(is_possible(train2, atoi(req_list[i].berths), 'a')==0)
                {
                    strcat(response_to_agent, ",Sorry! no berths available!\n");

                    cout<<"Generated resp - > "<<response_to_agent;

                    int rst = send (origin_of_msg, response_to_agent, strlen(response_to_agent), 0);
                    if (rst == -1)
                    {
                        //cerr<<"Hello1\n";
                        perror ("4. Server: Send failed");
                    }
                    else 
                    {
                        //cerr<<"rst -> "<<rst<<"\n";
                        printf ("Server 2 Client message sent successfully.\n");
                        fflush(stdout);
                    }

                    continue;

                    //goto end_part;
                    //continue;                    
                }
            }
            else if(strcmp(req_list[i].train,"12301")==0 && strcmp(req_list[i].coach_type,"Sleeper")==0)
            {
                if(is_possible(train2, atoi(req_list[i].berths), 's')==0)
                {
                    strcat(response_to_agent, ",Sorry! no berths available!\n");

                    cout<<"Generated resp - > "<<response_to_agent;

                    int rst = send (origin_of_msg, response_to_agent, strlen(response_to_agent), 0);
                    if (rst == -1)
                    {
                        //cerr<<"Hello1\n";
                        perror ("4. Server: Send failed");
                    }
                    else 
                    {
                        //cerr<<"rst -> "<<rst<<"\n";
                        printf ("Server 2 Client message sent successfully.\n");
                        fflush(stdout);
                    }
                        //goto end_part;
                        continue;                    
                }
            }

            //strcat(response_to_agent, ",");
            int su_req=0, sl_req=0, ub_req=0, mb_req=0, lb_req=0;
            if(strcmp(req_list[i].preferences,"NA")==0)         //when no preference is given by the passanger
            {
                cout<<"In NA part1\n";
                //cout<<"TYPE -> "<<req_list[i].coach_type<<"\n";
                //cout<<(strcmp(req_list[i].coach_type,"AC"))<<"\n";

                /*int coach_found ;
                if(strcmp(req_list[i].train,"12321")==0 && strcmp(req_list[i].coach_type,"AC")==0) coach_found = best_poss_coach_na(train1, 'a');
                else if(strcmp(req_list[i].train,"12321")==0 && strcmp(req_list[i].coach_type,"Sleeper")==0) coach_found = best_poss_coach_na(train1, 's');
                else if(strcmp(req_list[i].train,"12301")==0 && strcmp(req_list[i].coach_type,"AC")==0) 
                {
                    cout<<"This opt1\n";
                    coach_found = best_poss_coach_na(train2, 'a');
                    cout<<"val ret-> "<<coach_found<<"\n";
                }
                else if(strcmp(req_list[i].train,"12301")==0 && strcmp(req_list[i].coach_type,"Sleeper")==0) coach_found = best_poss_coach_na(train2, 's');
                
                cout<<"Best coach -> "<<coach_found<<"\n";*/

                int seats_rem = atoi(req_list[i].berths);
                int seat_no;

                if(strcmp(req_list[i].train,"12321")==0 && strcmp(req_list[i].coach_type,"AC")==0)
                {
                        for(i=1;i<=train1.ac_count && seats_rem>0 ;i++)
                        {
                            for(seat_no=1;seat_no<=72 && seats_rem>0 ;seat_no++)
                            {
                                if(seat_no%8==1)
                                {
                                    //SU
                                    if(train1.ac_coach_su[i][(seat_no/8)+1]==0)
                                    {
                                        train1.ac_coach_su[i][(seat_no/8)+1]=1;

                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);

                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%8==2)
                                {
                                    //SL
                                    if(train1.ac_coach_sl[i][(seat_no/8)+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train1.ac_coach_sl[i][(seat_no/8)+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%8==3)
                                {
                                    //UB
                                    if(train1.ac_coach_ub[i][((seat_no-3)/8)*2+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train1.ac_coach_ub[i][((seat_no-3)/8)*2+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%8==6)
                                {
                                    //UB
                                    if(train1.ac_coach_ub[i][((seat_no-6)/8)*2+2]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train1.ac_coach_ub[i][((seat_no-6)/8)*2+2]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%8==4)
                                {
                                    //MB
                                    if(train1.ac_coach_mb[i][((seat_no-4)/8)*2+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train1.ac_coach_mb[i][((seat_no-4)/8)*2+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%8==7)
                                {
                                    //MB
                                    if(train1.ac_coach_mb[i][((seat_no-7)/8)*2+2]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train1.ac_coach_mb[i][((seat_no-7)/8)*2+2]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%8==5)
                                {
                                    //LB
                                    if(train1.ac_coach_lb[i][((seat_no-5)/8)*2+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train1.ac_coach_lb[i][((seat_no-5)/8)*2+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%8==0)
                                {
                                    //LB
                                    if(train1.ac_coach_lb[i][((seat_no-8)/8)*2+2]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train1.ac_coach_lb[i][((seat_no-8)/8)*2+2]=1;
                                        seats_rem--; 
                                    }
                                }
                            }
                        }

                        /*for(i=1;i<=9 && seats_rem>0 && (train1.su ==1) ;i++)
                        {
                                if(train1.ac_coach_su[coach_found][i]==0)
                                {
                                    train1.ac_coach_su[coach_found][i]=1;
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",coach_found,8*(i-1)+1);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }

                        for(i=1;i<=9 && seats_rem>0 && (train1.sl ==1) ;i++)
                        {
                                if(train1.ac_coach_sl[coach_found][i]==0)
                                {
                                    train1.ac_coach_sl[coach_found][i]=1;
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",coach_found,8*(i-1)+2);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }

                        for(i=1;i<=18 && seats_rem>0 && (train1.ub ==1) ;i++)
                        {
                                if(train1.ac_coach_ub[coach_found][i]==0)
                                {
                                    train1.ac_coach_ub[coach_found][i]=1;
                                    char temp_str1[100];
                                    if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+3);
                                    else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+6);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }

                        for(i=1;i<=18 && seats_rem>0 && (train1.mb ==1) ;i++)
                        {
                                if(train1.ac_coach_mb[coach_found][i]==0)
                                {
                                    train1.ac_coach_mb[coach_found][i]=1;
                                    char temp_str1[100];
                                    if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+4);
                                    else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+7);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }

                        for(i=1;i<=18 && seats_rem>0 && (train1.lb ==1) ;i++)
                        {
                                if(train1.ac_coach_lb[coach_found][i]==0)
                                {
                                    train1.ac_coach_lb[coach_found][i]=1;
                                    char temp_str1[100];
                                    if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+5);
                                    else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+8);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }
                        

                        for(i=1;i<=train1.ac_count && seats_rem>0 ;i++)
                        {
                            for(j=1;j<=9 && seats_rem>0 && (train1.su ==1) ;j++)
                            {
                                    if(train1.ac_coach_su[i][j]==0)
                                    {
                                        train1.ac_coach_su[i][j]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",A%d-%d",i,8*(j-1)+1);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }

                            for(j=1;j<=9 && seats_rem>0 && (train1.sl ==1) ;j++)
                            {
                                    if(train1.ac_coach_sl[i][j]==0)
                                    {
                                        train1.ac_coach_sl[i][j]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",A%d-%d",i,8*(j-1)+2);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }

                            for(j=1;j<=18 && seats_rem>0 && (train1.ub ==1) ;j++)
                            {
                                    if(train1.ac_coach_ub[i][j]==0)
                                    {
                                        train1.ac_coach_ub[i][j]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+3);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+6);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }

                            for(j=1;j<=18 && seats_rem>0 && (train1.mb ==1) ;j++)
                            {
                                    if(train1.ac_coach_mb[i][j]==0)
                                    {
                                        train1.ac_coach_mb[i][j]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+7);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }

                            for(j=1;j<=18 && seats_rem>0 && (train1.lb ==1) ;j++)
                            {
                                    if(train1.ac_coach_lb[i][j]==0)
                                    {
                                        train1.ac_coach_lb[i][j]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+5);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+8);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }                            
                        }*/
                }
                else if (strcmp(req_list[i].train,"12321")==0 && strcmp(req_list[i].coach_type,"Sleeper")==0)
                {
                        //cout<<"->>>> here111\n";

                        for(i=1;i<=train1.sleeper_count && seats_rem>0 ;i++)
                        {
                            for(seat_no=1;seat_no<=72 && seats_rem>0 ;seat_no++)
                            {
                                if(seat_no%8==1)
                                {
                                    //SU
                                    if(train1.sleeper_coach_su[i][(seat_no/8)+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train1.sleeper_coach_su[i][(seat_no/8)+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%8==2)
                                {
                                    //SL
                                    if(train1.sleeper_coach_sl[i][(seat_no/8)+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train1.sleeper_coach_sl[i][(seat_no/8)+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%8==3)
                                {
                                    //UB
                                    if(train1.sleeper_coach_ub[i][((seat_no-3)/8)*2+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train1.sleeper_coach_ub[i][((seat_no-3)/8)*2+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%8==6)
                                {
                                    //UB
                                    if(train1.sleeper_coach_ub[i][((seat_no-6)/8)*2+2]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train1.sleeper_coach_ub[i][((seat_no-6)/8)*2+2]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%8==4)
                                {
                                    //MB
                                    if(train1.sleeper_coach_mb[i][((seat_no-4)/8)*2+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train1.sleeper_coach_mb[i][((seat_no-4)/8)*2+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%8==7)
                                {
                                    //MB
                                    if(train1.sleeper_coach_mb[i][((seat_no-7)/8)*2+2]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train1.sleeper_coach_mb[i][((seat_no-7)/8)*2+2]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%8==5)
                                {
                                    //LB
                                    if(train1.sleeper_coach_lb[i][((seat_no-5)/8)*2+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train1.sleeper_coach_lb[i][((seat_no-5)/8)*2+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%8==0)
                                {
                                    //LB
                                    if(train1.sleeper_coach_lb[i][((seat_no-8)/8)*2+2]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train1.sleeper_coach_lb[i][((seat_no-8)/8)*2+2]=1;
                                        seats_rem--; 
                                    }
                                }
                            }
                        }

                        /*for(i=1;i<=9 && seats_rem>0 && (train1.su ==1) ;i++)
                        {
                                if(train1.sleeper_coach_su[coach_found][i]==0)
                                {
                                    train1.ac_coach_su[coach_found][i]=1;
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",coach_found,8*(i-1)+1);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }

                        for(i=1;i<=9 && seats_rem>0 && (train1.sl ==1) ;i++)
                        {
                                if(train1.sleeper_coach_sl[coach_found][i]==0)
                                {
                                    train1.sleeper_coach_sl[coach_found][i]=1;
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",coach_found,8*(i-1)+2);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }

                        for(i=1;i<=18 && seats_rem>0 && (train1.ub ==1) ;i++)
                        {
                                if(train1.sleeper_coach_ub[coach_found][i]==0)
                                {
                                    train1.sleeper_coach_ub[coach_found][i]=1;
                                    char temp_str1[100];
                                    if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+3);
                                    else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+6);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }

                        for(i=1;i<=18 && seats_rem>0 && (train1.mb ==1) ;i++)
                        {
                                if(train1.sleeper_coach_mb[coach_found][i]==0)
                                {
                                    train1.sleeper_coach_mb[coach_found][i]=1;
                                    char temp_str1[100];
                                    if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+4);
                                    else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+7);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }

                        for(i=1;i<=18 && seats_rem>0 && (train1.lb ==1) ;i++)
                        {
                                if(train1.sleeper_coach_lb[coach_found][i]==0)
                                {
                                    train1.sleeper_coach_lb[coach_found][i]=1;
                                    char temp_str1[100];
                                    if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+5);
                                    else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+8);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }


                        for(i=1;i<=train1.sleeper_count && seats_rem>0 ;i++)
                        {
                            for(j=1;j<=9 && seats_rem>0 && (train1.su ==1) ;j++)
                            {
                                    if(train1.sleeper_coach_su[i][j]==0)
                                    {
                                        train1.sleeper_coach_su[i][j]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",S%d-%d",i,8*(j-1)+1);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }

                            for(j=1;j<=9 && seats_rem>0 && (train1.sl ==1) ;j++)
                            {
                                    if(train1.sleeper_coach_sl[i][j]==0)
                                    {
                                        train1.sleeper_coach_sl[i][j]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",S%d-%d",i,8*(j-1)+2);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }

                            for(j=1;j<=18 && seats_rem>0 && (train1.ub ==1) ;j++)
                            {
                                    if(train1.sleeper_coach_ub[i][j]==0)
                                    {
                                        train1.sleeper_coach_ub[i][j]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+3);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+6);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }

                            for(j=1;j<=18 && seats_rem>0 && (train1.mb ==1) ;j++)
                            {
                                    if(train1.sleeper_coach_mb[i][j]==0)
                                    {
                                        train1.sleeper_coach_mb[i][j]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+7);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }

                            for(j=1;j<=18 && seats_rem>0 && (train1.lb ==1) ;j++)
                            {
                                    if(train1.sleeper_coach_lb[i][j]==0)
                                    {
                                        train1.sleeper_coach_lb[i][j]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+5);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+8);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }                            
                        }  */                    
                }
                else if(strcmp(req_list[i].train,"12301")==0 && strcmp(req_list[i].coach_type,"AC")==0)
                {
                        //cout<<"here3333\n";

                        for(i=1;i<=train2.ac_count && seats_rem>0 ;i++)
                        {
                            for(seat_no=1;seat_no<=54 && seats_rem>0 ;seat_no++)
                            {
                                if(seat_no%6==1)
                                {
                                    //SU
                                    if(train2.ac_coach_su[i][(seat_no/6)+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train2.ac_coach_su[i][(seat_no/6)+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%6==2)
                                {
                                    //SL
                                    if(train2.ac_coach_sl[i][(seat_no/6)+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train2.ac_coach_sl[i][(seat_no/6)+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%6==3)
                                {
                                    //UB
                                    if(train2.ac_coach_ub[i][((seat_no-3)/6)*2+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train2.ac_coach_ub[i][((seat_no-3)/6)*2+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%6==5)
                                {
                                    //UB
                                    if(train2.ac_coach_ub[i][((seat_no-5)/6)*2+2]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train2.ac_coach_ub[i][((seat_no-5)/6)*2+2]=1;
                                        seats_rem--; 
                                    }
                                }
                                /*else if(seat_no%8==4)
                                {
                                    //MB
                                    if(train2.ac_coach_mb[i][((seat_no-4)/8)*2+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train2.ac_coach_mb[i][((seat_no-4)/8)*2+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%8==7)
                                {
                                    //MB
                                    if(train2.ac_coach_mb[i][((seat_no-7)/8)*2+2]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train2.ac_coach_mb[i][((seat_no-7)/8)*2+2]=1;
                                        seats_rem--; 
                                    }
                                }*/
                                else if(seat_no%6==4)
                                {
                                    //LB
                                    if(train2.ac_coach_lb[i][((seat_no-4)/6)*2+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train2.ac_coach_lb[i][((seat_no-4)/6)*2+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%6==0)
                                {
                                    //LB
                                    if(train2.ac_coach_lb[i][((seat_no-6)/6)*2+2]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train2.ac_coach_lb[i][((seat_no-6)/6)*2+2]=1;
                                        seats_rem--; 
                                    }
                                }
                            }
                        }

                        /*for(i=1;i<=9 && seats_rem>0 && (train2.su ==1) ;i++)
                        {
                                if(train2.ac_coach_su[coach_found][i]==0)
                                {
                                    train2.ac_coach_su[coach_found][i]=1;
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",coach_found,6*(i-1)+1);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }

                        for(i=1;i<=9 && seats_rem>0 && (train2.sl ==1) ;i++)
                        {
                                if(train2.ac_coach_sl[coach_found][i]==0)
                                {
                                    train2.ac_coach_sl[coach_found][i]=1;
                                    char temp_str1[100];
                                    sprintf(temp_str1,",A%d-%d",coach_found,6*(i-1)+2);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }

                        for(i=1;i<=18 && seats_rem>0 && (train2.ub ==1) ;i++)
                        {
                                if(train2.ac_coach_ub[coach_found][i]==0)
                                {
                                    train2.ac_coach_ub[coach_found][i]=1;
                                    char temp_str1[100];
                                    if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,6*((i-1)/2)+3);
                                    else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,6*((i-1)/2)+5);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }

                        /*for(i=1;i<=9 && seats_rem>0 && (train2.mb ==1) ;i++)
                        {
                                if(train2.ac_coach_mb[coach_found][i]==0)
                                {
                                    train2.ac_coach_mb[coach_found][i]=1;
                                    char temp_str1[100];
                                    if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+4);
                                    else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+7);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }*/

                        /*for(i=1;i<=18 && seats_rem>0 && (train2.lb ==1) ;i++)
                        {
                                if(train2.ac_coach_lb[coach_found][i]==0)
                                {
                                    train2.ac_coach_lb[coach_found][i]=1;
                                    char temp_str1[100];
                                    if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,6*((i-1)/2)+4);
                                    else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,6*((i-1)/2)+6);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }


                        for(i=1;i<=train2.ac_count && seats_rem>0 ;i++)
                        {
                            for(j=1;j<=9 && seats_rem>0 && (train2.su ==1) ;j++)
                            {
                                    if(train2.ac_coach_su[i][j]==0)
                                    {
                                        train2.ac_coach_su[i][j]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",A%d-%d",i,6*(j-1)+1);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }

                            for(j=1;j<=9 && seats_rem>0 && (train2.sl ==1) ;j++)
                            {
                                    if(train2.ac_coach_sl[i][j]==0)
                                    {
                                        train2.ac_coach_sl[i][j]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",A%d-%d",i,6*(j-1)+2);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }

                            for(j=1;j<=18 && seats_rem>0 && (train2.ub ==1) ;j++)
                            {
                                    if(train2.ac_coach_ub[i][j]==0)
                                    {
                                        train2.ac_coach_ub[i][j]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",i,6*((j-1)/2)+3);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,6*((j-1)/2)+5);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }*/

                            /*for(j=1;j<=9 && seats_rem>0 && (train2.mb ==1) ;j++)
                            {
                                    if(train2.ac_coach_mb[i][j]==0)
                                    {
                                        train2.ac_coach_mb[i][j]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+7);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }*/

                            /*for(j=1;j<=18 && seats_rem>0 && (train2.lb ==1) ;j++)
                            {
                                    if(train2.ac_coach_lb[i][j]==0)
                                    {
                                        train2.ac_coach_lb[i][j]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",i,6*((j-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,6*((j-1)/2)+6);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }                            
                        }*/
                }
                else if(strcmp(req_list[i].train,"12301")==0 && strcmp(req_list[i].coach_type,"Sleeper")==0)
                {

                        strcat(response_to_agent, ",Sorry! no berths available!\n");

                        cout<<"Generated resp - > "<<response_to_agent;

                        int rst = send (origin_of_msg, response_to_agent, strlen(response_to_agent), 0);
                        if (rst == -1)
                        {
                            //cerr<<"Hello1\n";
                            perror ("4. Server: Send failed");
                        }
                        else 
                        {
                            //cerr<<"rst -> "<<rst<<"\n";
                            printf ("S2C message sent successfully.\n");
                            fflush(stdout);
                        }
                            //goto end_part;
                            continue;                   



                        for(i=1;i<=train2.sleeper_count && seats_rem>0 ;i++)
                        {
                            for(seat_no=1;seat_no<=54 && seats_rem>0 ;seat_no++)
                            {
                                if(seat_no%6==1)
                                {
                                    //SU
                                    if(train2.sleeper_coach_su[i][(seat_no/6)+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train2.sleeper_coach_su[i][(seat_no/6)+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%6==2)
                                {
                                    //SL
                                    if(train2.sleeper_coach_sl[i][(seat_no/6)+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train2.sleeper_coach_sl[i][(seat_no/6)+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%6==3)
                                {
                                    //UB
                                    if(train2.sleeper_coach_ub[i][((seat_no-3)/6)*2+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train2.sleeper_coach_ub[i][((seat_no-3)/6)*2+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%6==5)
                                {
                                    //UB
                                    if(train2.sleeper_coach_ub[i][((seat_no-5)/6)*2+2]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train2.sleeper_coach_ub[i][((seat_no-5)/6)*2+2]=1;
                                        seats_rem--; 
                                    }
                                }
                                /*else if(seat_no%8==4)
                                {
                                    //MB
                                    if(train2.sleeper_coach_mb[i][((seat_no-4)/8)*2+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train2.sleeper_coach_mb[i][((seat_no-4)/8)*2+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%8==7)
                                {
                                    //MB
                                    if(train2.sleeper_coach_mb[i][((seat_no-7)/8)*2+2]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train2.sleeper_coach_mb[i][((seat_no-7)/8)*2+2]=1;
                                        seats_rem--; 
                                    }
                                }*/
                                else if(seat_no%6==4)
                                {
                                    //LB
                                    if(train2.sleeper_coach_lb[i][((seat_no-4)/6)*2+1]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train2.sleeper_coach_lb[i][((seat_no-4)/6)*2+1]=1;
                                        seats_rem--; 
                                    }
                                }
                                else if(seat_no%6==0)
                                {
                                    //LB
                                    if(train2.sleeper_coach_lb[i][((seat_no-6)/6)*2+2]==0)
                                    {
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",i,seat_no);
                                    strcat(response_to_agent, temp_str1);
                                        train2.sleeper_coach_lb[i][((seat_no-6)/6)*2+2]=1;
                                        seats_rem--; 
                                    }
                                }
                            }
                        }
                        
                        /*for(i=1;i<=9 && seats_rem>0 && (train2.su ==1) ;i++)
                        {
                                if(train2.sleeper_coach_su[coach_found][i]==0)
                                {
                                    train2.sleeper_coach_su[coach_found][i]=1;
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",coach_found,6*(i-1)+1);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }

                        for(i=1;i<=9 && seats_rem>0 && (train2.sl ==1) ;i++)
                        {
                                if(train2.sleeper_coach_sl[coach_found][i]==0)
                                {
                                    train2.sleeper_coach_sl[coach_found][i]=1;
                                    char temp_str1[100];
                                    sprintf(temp_str1,",S%d-%d",coach_found,6*(i-1)+2);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }

                        for(i=1;i<=18 && seats_rem>0 && (train2.ub ==1) ;i++)
                        {
                                if(train2.sleeper_coach_ub[coach_found][i]==0)
                                {
                                    train2.sleeper_coach_ub[coach_found][i]=1;
                                    char temp_str1[100];
                                    if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,6*((i-1)/2)+3);
                                    else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,6*((i-1)/2)+5);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }*/

                        /*for(i=1;i<=9 && seats_rem>0 && (train2.mb ==1) ;i++)
                        {
                                if(train2.sleeper_coach_mb[coach_found][i]==0)
                                {
                                    train2.sleeper_coach_mb[coach_found][i]=1;
                                    char temp_str1[100];
                                    if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+4);
                                    else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+7);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }*/

                        /*for(i=1;i<=18 && seats_rem>0 && (train2.lb ==1) ;i++)
                        {
                                if(train2.sleeper_coach_lb[coach_found][i]==0)
                                {
                                    train2.sleeper_coach_lb[coach_found][i]=1;
                                    char temp_str1[100];
                                    if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,6*((i-1)/2)+4);
                                    else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,6*((i-1)/2)+6);
                                    strcat(response_to_agent, temp_str1);
                                    seats_rem--;
                                }
                        }


                        for(i=1;i<=train2.sleeper_count && seats_rem>0 ;i++)
                        {
                            for(j=1;j<=9 && seats_rem>0 && (train2.su ==1) ;j++)
                            {
                                    if(train2.sleeper_coach_su[i][j]==0)
                                    {
                                        train2.sleeper_coach_su[i][j]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",S%d-%d",i,6*(j-1)+1);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }

                            for(j=1;j<=9 && seats_rem>0 && (train2.sl ==1) ;j++)
                            {
                                    if(train2.sleeper_coach_sl[i][j]==0)
                                    {
                                        train2.sleeper_coach_sl[i][j]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",S%d-%d",i,6*(j-1)+2);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }

                            for(j=1;j<=18 && seats_rem>0 && (train2.ub ==1) ;j++)
                            {
                                    if(train2.sleeper_coach_ub[i][j]==0)
                                    {
                                        train2.sleeper_coach_ub[i][j]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",i,6*((j-1)/2)+3);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",i,6*((j-1)/2)+5);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }*/

                            /*for(j=1;j<=9 && seats_rem>0 && (train2.mb ==1) ;j++)
                            {
                                    if(train2.sleeper_coach_mb[i][j]==0)
                                    {
                                        train2.sleeper_coach_mb[i][j]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+7);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }*/

                            /*for(j=1;j<=18 && seats_rem>0 && (train2.lb ==1) ;j++)
                            {
                                    if(train2.sleeper_coach_lb[i][j]==0)
                                    {
                                        train2.sleeper_coach_lb[i][j]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",i,6*((j-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",i,6*((j-1)/2)+6);
                                        strcat(response_to_agent, temp_str1);
                                        seats_rem--;
                                    }
                            }                            
                        }*/
                }

            //end of NA part
            }
            else if(strcmp(req_list[i].preferences,"NA")!=0)    //when preferences given by the passanger is not NA
            {
                //cout<<"Here55\n";
                su_req = count_occurences(req_list[i].preferences,"SU");
                sl_req = count_occurences(req_list[i].preferences,"SL");
                ub_req = count_occurences(req_list[i].preferences,"UB");
                mb_req = count_occurences(req_list[i].preferences,"MB");
                lb_req = count_occurences(req_list[i].preferences,"LB");

                cout<<"REQ - > "<<su_req<<"\t"<<sl_req<<"\t"<<ub_req<<"\t"<<mb_req<<"\t"<<lb_req<<"\n";

                if(strcmp(req_list[i].train,"12321")==0)
                {
                    if(strcmp(req_list[i].coach_type,"AC")==0)
                    {
                        //cout<<"Here66\n";
                        int coach_found = 0;
                        //if((coach_found = is_suitable(train1.ac_coach_su,train1.ac_coach_sl,train1.ac_coach_ub,train1.ac_coach_mb,train1.ac_coach_lb,su_req,sl_req,ub_req,mb_req,lb_req,train1.ac_count))!=(-1))
                        if((coach_found = is_suitable(train1,su_req,sl_req,ub_req,mb_req,lb_req,'a'))!=(-1))
                        {
                            //cout<<"Single poss1\n";
                            for(i=1;i<=9 && su_req>0 && (train1.su ==1) ;i++)
                            {
                                    if(train1.ac_coach_su[coach_found][i]==0)
                                    {
                                        train1.ac_coach_su[coach_found][i]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",A%d-%d",coach_found,8*(i-1)+1);
                                        strcat(response_to_agent, temp_str1);
                                        //cout<<"Mod resp - > "<<response_to_agent<<"\n";
                                        su_req--;
                                    }
                            }

                            for(i=1;i<=9 && sl_req>0 && (train1.sl ==1) ;i++)
                            {
                                    if(train1.ac_coach_sl[coach_found][i]==0)
                                    {
                                        train1.ac_coach_sl[coach_found][i]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",A%d-%d",coach_found,8*(i-1)+2);
                                        strcat(response_to_agent, temp_str1);
                                        //cout<<"Mod resp - > "<<response_to_agent<<"\n";
                                        sl_req--;
                                    }
                            }

                            for(i=1;i<=18 && ub_req>0 && (train1.ub ==1) ;i++)
                            {
                                    if(train1.ac_coach_ub[coach_found][i]==0)
                                    {
                                        train1.ac_coach_ub[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+3);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+6);
                                        strcat(response_to_agent, temp_str1);
                                        //cout<<"Mod resp - > "<<response_to_agent<<"\n";
                                        ub_req--;
                                    }
                            }

                            for(i=1;i<=18 && mb_req>0 && (train1.mb ==1) ;i++)
                            {
                                    if(train1.ac_coach_mb[coach_found][i]==0)
                                    {
                                        train1.ac_coach_mb[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+7);
                                        strcat(response_to_agent, temp_str1);
                                        //cout<<"Mod resp - > "<<response_to_agent<<"\n";
                                        mb_req--;
                                    }
                            }

                            for(i=1;i<=18 && lb_req>0 && (train1.lb ==1) ;i++)
                            {
                                    if(train1.ac_coach_lb[coach_found][i]==0)
                                    {
                                        train1.ac_coach_lb[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+5);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+8);
                                        strcat(response_to_agent, temp_str1);
                                        //cout<<"Mod resp - > "<<response_to_agent<<"\n";
                                        lb_req--;
                                    }
                            }
                        }
                        else
                        {
                            //coach_found = best_poss_coach(train1.ac_coach_su,train1.ac_coach_sl,train1.ac_coach_ub,train1.ac_coach_mb,train1.ac_coach_lb,su_req,sl_req,ub_req,mb_req,lb_req,train1.ac_count);
                            coach_found = best_poss_coach(train1,su_req,sl_req,ub_req,mb_req,lb_req,'a');

                            cout<<"Best coach AC ->"<<coach_found<<"\n";

                            for(i=1;i<=9 && su_req>0 && (train1.su ==1) ;i++)
                            {
                                    if(train1.ac_coach_su[coach_found][i]==0)
                                    {
                                        train1.ac_coach_su[coach_found][i]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",A%d-%d",coach_found,8*(i-1)+1);
                                        strcat(response_to_agent, temp_str1);
                                        su_req--;
                                    }
                            }

                            for(i=1;i<=9 && sl_req>0 && (train1.sl ==1) ;i++)
                            {
                                    if(train1.ac_coach_sl[coach_found][i]==0)
                                    {
                                        train1.ac_coach_sl[coach_found][i]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",A%d-%d",coach_found,8*(i-1)+2);
                                        strcat(response_to_agent, temp_str1);
                                        sl_req--;
                                    }
                            }

                            for(i=1;i<=18 && ub_req>0 && (train1.ub ==1) ;i++)
                            {
                                    if(train1.ac_coach_ub[coach_found][i]==0)
                                    {
                                        train1.ac_coach_ub[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+3);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+6);
                                        strcat(response_to_agent, temp_str1);
                                        ub_req--;
                                    }
                            }

                            for(i=1;i<=18 && mb_req>0 && (train1.mb ==1) ;i++)
                            {
                                    if(train1.ac_coach_mb[coach_found][i]==0)
                                    {
                                        train1.ac_coach_mb[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+7);
                                        strcat(response_to_agent, temp_str1);
                                        mb_req--;
                                    }
                            }

                            for(i=1;i<=18 && lb_req>0 && (train1.lb ==1) ;i++)
                            {
                                    if(train1.ac_coach_lb[coach_found][i]==0)
                                    {
                                        train1.ac_coach_lb[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+5);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+8);
                                        strcat(response_to_agent, temp_str1);
                                        lb_req--;
                                    }
                            }


                            for(i=coach_found;i<=train1.ac_count && (su_req||sl_req||ub_req||mb_req||lb_req) ;i++)
                            {
                                if(i==coach_found)
                                {
                                    int seats_rem = su_req+sl_req+ub_req+mb_req+lb_req;
                                    for(int seat_no=1;seat_no<=72 && seats_rem>0 ;seat_no++)
                                    {
                                        if(seat_no%8==1)
                                        {
                                            //SU
                                            if(train1.ac_coach_su[i][(seat_no/8)+1]==0)
                                            {
                                                train1.ac_coach_su[i][(seat_no/8)+1]=1;

                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);

                                                seats_rem--; 
                                                //su_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==2)
                                        {
                                            //SL
                                            if(train1.ac_coach_sl[i][(seat_no/8)+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_sl[i][(seat_no/8)+1]=1;
                                                seats_rem--; 
                                                //sl_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==3)
                                        {
                                            //UB
                                            if(train1.ac_coach_ub[i][((seat_no-3)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_ub[i][((seat_no-3)/8)*2+1]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==6)
                                        {
                                            //UB
                                            if(train1.ac_coach_ub[i][((seat_no-6)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_ub[i][((seat_no-6)/8)*2+2]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==4)
                                        {
                                            //MB
                                            if(train1.ac_coach_mb[i][((seat_no-4)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_mb[i][((seat_no-4)/8)*2+1]=1;
                                                seats_rem--; 
                                                //mb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==7)
                                        {
                                            //MB
                                            if(train1.ac_coach_mb[i][((seat_no-7)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_mb[i][((seat_no-7)/8)*2+2]=1;
                                                seats_rem--;
                                                //mb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==5)
                                        {
                                            //LB
                                            if(train1.ac_coach_lb[i][((seat_no-5)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_lb[i][((seat_no-5)/8)*2+1]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==0)
                                        {
                                            //LB
                                            if(train1.ac_coach_lb[i][((seat_no-8)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_lb[i][((seat_no-8)/8)*2+2]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                    }                                    
                                }
                                else
                                {
                                    for(j=1;j<=9 && su_req>0 && (train1.su ==1) ;j++)
                                    {
                                            if(train1.ac_coach_su[i][j]==0)
                                            {
                                                train1.ac_coach_su[i][j]=1;
                                                char temp_str1[100];
                                                sprintf(temp_str1,",A%d-%d",i,8*(j-1)+1);
                                                strcat(response_to_agent, temp_str1);
                                                su_req--;
                                            }
                                    }

                                    for(j=1;j<=9 && sl_req>0 && (train1.sl ==1) ;j++)
                                    {
                                            if(train1.ac_coach_sl[i][j]==0)
                                            {
                                                train1.ac_coach_sl[i][j]=1;
                                                char temp_str1[100];
                                                sprintf(temp_str1,",A%d-%d",i,8*(j-1)+2);
                                                strcat(response_to_agent, temp_str1);
                                                sl_req--;
                                            }
                                    }

                                    for(j=1;j<=18 && ub_req>0 && (train1.ub ==1) ;j++)
                                    {
                                            if(train1.ac_coach_ub[i][j]==0)
                                            {
                                                train1.ac_coach_ub[i][j]=1;
                                                char temp_str1[100];
                                                if(i%2==1) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+3);
                                                else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+6);
                                                strcat(response_to_agent, temp_str1);
                                                ub_req--;
                                            }
                                    }

                                    for(j=1;j<=18 && mb_req>0 && (train1.mb ==1) ;j++)
                                    {
                                            if(train1.ac_coach_mb[i][j]==0)
                                            {
                                                train1.ac_coach_mb[i][j]=1;
                                                char temp_str1[100];
                                                if(i%2==1) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+4);
                                                else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+7);
                                                strcat(response_to_agent, temp_str1);
                                                mb_req--;
                                            }
                                    }

                                    for(j=1;j<=18 && lb_req>0 && (train1.lb ==1) ;j++)
                                    {
                                            if(train1.ac_coach_lb[i][j]==0)
                                            {
                                                train1.ac_coach_lb[i][j]=1;
                                                char temp_str1[100];
                                                if(i%2==1) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+5);
                                                else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+8);
                                                strcat(response_to_agent, temp_str1);
                                                lb_req--;
                                            }
                                    }   


                                    int seats_rem = su_req+sl_req+ub_req+mb_req+lb_req;
                                    for(int seat_no=1;seat_no<=72 && seats_rem>0 ;seat_no++)
                                    {
                                        if(seat_no%8==1)
                                        {
                                            //SU
                                            if(train1.ac_coach_su[i][(seat_no/8)+1]==0)
                                            {
                                                train1.ac_coach_su[i][(seat_no/8)+1]=1;

                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);

                                                seats_rem--; 
                                                //su_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==2)
                                        {
                                            //SL
                                            if(train1.ac_coach_sl[i][(seat_no/8)+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_sl[i][(seat_no/8)+1]=1;
                                                seats_rem--; 
                                                //sl_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==3)
                                        {
                                            //UB
                                            if(train1.ac_coach_ub[i][((seat_no-3)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_ub[i][((seat_no-3)/8)*2+1]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==6)
                                        {
                                            //UB
                                            if(train1.ac_coach_ub[i][((seat_no-6)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_ub[i][((seat_no-6)/8)*2+2]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==4)
                                        {
                                            //MB
                                            if(train1.ac_coach_mb[i][((seat_no-4)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_mb[i][((seat_no-4)/8)*2+1]=1;
                                                seats_rem--; 
                                                //mb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==7)
                                        {
                                            //MB
                                            if(train1.ac_coach_mb[i][((seat_no-7)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_mb[i][((seat_no-7)/8)*2+2]=1;
                                                seats_rem--;
                                                //mb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==5)
                                        {
                                            //LB
                                            if(train1.ac_coach_lb[i][((seat_no-5)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_lb[i][((seat_no-5)/8)*2+1]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==0)
                                        {
                                            //LB
                                            if(train1.ac_coach_lb[i][((seat_no-8)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_lb[i][((seat_no-8)/8)*2+2]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                    }
                                }                         
                            }



                            for(i=1;i<coach_found && (su_req||sl_req||ub_req||mb_req||lb_req) ;i++)
                            {
                                for(j=1;j<=9 && su_req>0 && (train1.su ==1) ;j++)
                                {
                                        if(train1.ac_coach_su[i][j]==0)
                                        {
                                            train1.ac_coach_su[i][j]=1;
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,8*(j-1)+1);
                                            strcat(response_to_agent, temp_str1);
                                            su_req--;
                                        }
                                }

                                for(j=1;j<=9 && sl_req>0 && (train1.sl ==1) ;j++)
                                {
                                        if(train1.ac_coach_sl[i][j]==0)
                                        {
                                            train1.ac_coach_sl[i][j]=1;
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,8*(j-1)+2);
                                            strcat(response_to_agent, temp_str1);
                                            sl_req--;
                                        }
                                }

                                for(j=1;j<=18 && ub_req>0 && (train1.ub ==1) ;j++)
                                {
                                        if(train1.ac_coach_ub[i][j]==0)
                                        {
                                            train1.ac_coach_ub[i][j]=1;
                                            char temp_str1[100];
                                            if(i%2==1) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+3);
                                            else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+6);
                                            strcat(response_to_agent, temp_str1);
                                            ub_req--;
                                        }
                                }

                                for(j=1;j<=18 && mb_req>0 && (train1.mb ==1) ;j++)
                                {
                                        if(train1.ac_coach_mb[i][j]==0)
                                        {
                                            train1.ac_coach_mb[i][j]=1;
                                            char temp_str1[100];
                                            if(i%2==1) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+4);
                                            else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+7);
                                            strcat(response_to_agent, temp_str1);
                                            mb_req--;
                                        }
                                }

                                for(j=1;j<=18 && lb_req>0 && (train1.lb ==1) ;j++)
                                {
                                        if(train1.ac_coach_lb[i][j]==0)
                                        {
                                            train1.ac_coach_lb[i][j]=1;
                                            char temp_str1[100];
                                            if(i%2==1) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+5);
                                            else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+8);
                                            strcat(response_to_agent, temp_str1);
                                            lb_req--;
                                        }
                                }    


                                int seats_rem = su_req+sl_req+ub_req+mb_req+lb_req;     
                                    for(int seat_no=1;seat_no<=72 && seats_rem>0 ;seat_no++)
                                    {
                                        if(seat_no%8==1)
                                        {
                                            //SU
                                            if(train1.ac_coach_su[i][(seat_no/8)+1]==0)
                                            {
                                                train1.ac_coach_su[i][(seat_no/8)+1]=1;

                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);

                                                seats_rem--; 
                                                //su_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==2)
                                        {
                                            //SL
                                            if(train1.ac_coach_sl[i][(seat_no/8)+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_sl[i][(seat_no/8)+1]=1;
                                                seats_rem--; 
                                                //sl_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==3)
                                        {
                                            //UB
                                            if(train1.ac_coach_ub[i][((seat_no-3)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_ub[i][((seat_no-3)/8)*2+1]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==6)
                                        {
                                            //UB
                                            if(train1.ac_coach_ub[i][((seat_no-6)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_ub[i][((seat_no-6)/8)*2+2]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==4)
                                        {
                                            //MB
                                            if(train1.ac_coach_mb[i][((seat_no-4)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_mb[i][((seat_no-4)/8)*2+1]=1;
                                                seats_rem--; 
                                                //mb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==7)
                                        {
                                            //MB
                                            if(train1.ac_coach_mb[i][((seat_no-7)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_mb[i][((seat_no-7)/8)*2+2]=1;
                                                seats_rem--;
                                                //mb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==5)
                                        {
                                            //LB
                                            if(train1.ac_coach_lb[i][((seat_no-5)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_lb[i][((seat_no-5)/8)*2+1]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==0)
                                        {
                                            //LB
                                            if(train1.ac_coach_lb[i][((seat_no-8)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_lb[i][((seat_no-8)/8)*2+2]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                    }
                            }





                        //else ends here
                        }               
                        //ac part ends here
                    }
                    else if(strcmp(req_list[i].coach_type,"Sleeper")==0)
                    {
                        //cout<<"Case 123\n";
                        int coach_found = 0;
                        //if((coach_found = is_suitable(train1.sleeper_coach_su,train1.sleeper_coach_sl,train1.sleeper_coach_ub,train1.sleeper_coach_mb,train1.sleeper_coach_lb,su_req,sl_req,ub_req,mb_req,lb_req,train1.sleeper_count))!=(-1))
                        if((coach_found = is_suitable(train1,su_req,sl_req,ub_req,mb_req,lb_req,'s'))!=(-1))
                        {
                            cout<<"coach_found - > "<<coach_found<<"\n";
                            for(i=1;i<=9 && su_req>0 && (train1.su ==1) ;i++)
                            {
                                //cout<<"case 777\n";
                                    if(train1.sleeper_coach_su[coach_found][i]==0)
                                    {
                                        train1.sleeper_coach_su[coach_found][i]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",S%d-%d",coach_found,8*(i-1)+1);
                                        strcat(response_to_agent, temp_str1);
                                        su_req--;
                                    }
                            }

                            for(i=1;i<=9 && sl_req>0 && (train1.sl ==1) ;i++)
                            {
                                //cout<<"case 887\n";
                                    if(train1.sleeper_coach_sl[coach_found][i]==0)
                                    {
                                        train1.sleeper_coach_sl[coach_found][i]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",S%d-%d",coach_found,8*(i-1)+2);
                                        strcat(response_to_agent, temp_str1);
                                        sl_req--;
                                    }
                            }

                            for(i=1;i<=18 && ub_req>0 && (train1.ub ==1) ;i++)
                            {
                                //cout<<"case 997\n";
                                    if(train1.sleeper_coach_ub[coach_found][i]==0)
                                    {
                                        train1.sleeper_coach_ub[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+3);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+6);
                                        strcat(response_to_agent, temp_str1);
                                        ub_req--;
                                    }
                            }

                            for(i=1;i<=18 && mb_req>0 && (train1.mb ==1) ;i++)
                            {
                                //cout<<"case 007\n";
                                    if(train1.sleeper_coach_mb[coach_found][i]==0)
                                    {
                                        train1.sleeper_coach_mb[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+7);
                                        strcat(response_to_agent, temp_str1);
                                        mb_req--;
                                    }
                            }

                            for(i=1;i<=18 && lb_req>0 && (train1.lb ==1) ;i++)
                            {
                                //cout<<"case 227\n";
                                    if(train1.sleeper_coach_lb[coach_found][i]==0)
                                    {
                                        train1.sleeper_coach_lb[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+5);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+8);
                                        strcat(response_to_agent, temp_str1);
                                        lb_req--;
                                    }
                            }
                        }
                        else
                        {
                            //cout<<"not is_suitable\n";
                            //coach_found = best_poss_coach(train1.sleeper_coach_su,train1.sleeper_coach_sl,train1.sleeper_coach_ub,train1.sleeper_coach_mb,train1.sleeper_coach_lb,su_req,sl_req,ub_req,mb_req,lb_req,train1.sleeper_count);
                            coach_found = best_poss_coach(train1,su_req,sl_req,ub_req,mb_req,lb_req,'s');
                            cout<<"best_poss_coach -> "<<coach_found<<"\n";

                            for(i=1;i<=9 && su_req>0 && (train1.su ==1) ;i++)
                            {
                                    if(train1.sleeper_coach_su[coach_found][i]==0)
                                    {
                                        train1.sleeper_coach_su[coach_found][i]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",S%d-%d",coach_found,8*(i-1)+1);
                                        strcat(response_to_agent, temp_str1);
                                        su_req--;
                                    }
                            }

                            for(i=1;i<=9 && sl_req>0 && (train1.sl ==1) ;i++)
                            {
                                    if(train1.sleeper_coach_sl[coach_found][i]==0)
                                    {
                                        train1.sleeper_coach_sl[coach_found][i]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",S%d-%d",coach_found,8*(i-1)+2);
                                        strcat(response_to_agent, temp_str1);
                                        sl_req--;
                                    }
                            }

                            for(i=1;i<=18 && ub_req>0 && (train1.ub ==1) ;i++)
                            {
                                    if(train1.sleeper_coach_ub[coach_found][i]==0)
                                    {
                                        train1.sleeper_coach_ub[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+3);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+6);
                                        strcat(response_to_agent, temp_str1);
                                        ub_req--;
                                    }
                            }

                            for(i=1;i<=18 && mb_req>0 && (train1.mb ==1) ;i++)
                            {
                                    if(train1.sleeper_coach_mb[coach_found][i]==0)
                                    {
                                        train1.sleeper_coach_mb[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+7);
                                        strcat(response_to_agent, temp_str1);
                                        mb_req--;
                                    }
                            }

                            for(i=1;i<=18 && lb_req>0 && (train1.lb ==1) ;i++)
                            {
                                    if(train1.sleeper_coach_lb[coach_found][i]==0)
                                    {
                                        train1.sleeper_coach_lb[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+5);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+8);
                                        strcat(response_to_agent, temp_str1);
                                        lb_req--;
                                    }
                            }


                            for(i=coach_found;i<=train1.sleeper_count && (su_req>0||sl_req>0||ub_req>0||mb_req>0||lb_req>0) ;i++)
                            {
                                //cout<<" I-> "<<i<<"\n";
                                cout<<su_req<<" "<<sl_req<<" "<<ub_req<<" "<<mb_req<<" "<<lb_req<<"\n";
                                if(i==coach_found)
                                {
                                    int seats_rem = su_req+sl_req+ub_req+mb_req+lb_req;
                                    for(int seat_no=1;seat_no<=72 && seats_rem>0 ;seat_no++)
                                    {
                                        if(seat_no%8==1)
                                        {
                                            //SU
                                            if(train1.sleeper_coach_su[i][(seat_no/8)+1]==0)
                                            {
                                                train1.sleeper_coach_su[i][(seat_no/8)+1]=1;

                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);

                                                seats_rem--; 
                                                //su_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==2)
                                        {
                                            //SL
                                            if(train1.sleeper_coach_sl[i][(seat_no/8)+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_sl[i][(seat_no/8)+1]=1;
                                                seats_rem--; 
                                                //sl_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==3)
                                        {
                                            //UB
                                            if(train1.sleeper_coach_ub[i][((seat_no-3)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_ub[i][((seat_no-3)/8)*2+1]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==6)
                                        {
                                            //UB
                                            if(train1.sleeper_coach_ub[i][((seat_no-6)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_ub[i][((seat_no-6)/8)*2+2]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==4)
                                        {
                                            //MB
                                            if(train1.sleeper_coach_mb[i][((seat_no-4)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_mb[i][((seat_no-4)/8)*2+1]=1;
                                                seats_rem--; 
                                                //mb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==7)
                                        {
                                            //MB
                                            if(train1.sleeper_coach_mb[i][((seat_no-7)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_mb[i][((seat_no-7)/8)*2+2]=1;
                                                seats_rem--;
                                                //mb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==5)
                                        {
                                            //LB
                                            if(train1.sleeper_coach_lb[i][((seat_no-5)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_lb[i][((seat_no-5)/8)*2+1]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==0)
                                        {
                                            //LB
                                            if(train1.sleeper_coach_lb[i][((seat_no-8)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_lb[i][((seat_no-8)/8)*2+2]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                    }                                    
                                }
                                else
                                {
                                    for(j=1;j<=9 && su_req>0 && (train1.su ==1) ;j++)
                                    {
                                            if(train1.sleeper_coach_su[i][j]==0)
                                            {
                                                train1.sleeper_coach_su[i][j]=1;
                                                char temp_str1[100];
                                                sprintf(temp_str1,",S%d-%d",i,8*(j-1)+1);
                                                strcat(response_to_agent, temp_str1);
                                                su_req--;
                                            }
                                    }

                                    for(j=1;j<=9 && sl_req>0 && (train1.sl ==1) ;j++)
                                    {
                                            if(train1.sleeper_coach_sl[i][j]==0)
                                            {
                                                train1.sleeper_coach_sl[i][j]=1;
                                                char temp_str1[100];
                                                sprintf(temp_str1,",S%d-%d",i,8*(j-1)+2);
                                                strcat(response_to_agent, temp_str1);
                                                sl_req--;
                                            }
                                    }

                                    for(j=1;j<=18 && ub_req>0 && (train1.ub ==1) ;j++)
                                    {
                                            if(train1.sleeper_coach_ub[i][j]==0)
                                            {
                                                train1.sleeper_coach_ub[i][j]=1;
                                                char temp_str1[100];
                                                if(i%2==1) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+3);
                                                else if(i%2==0) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+6);
                                                strcat(response_to_agent, temp_str1);
                                                ub_req--;
                                            }
                                    }

                                    for(j=1;j<=18 && mb_req>0 && (train1.mb ==1) ;j++)
                                    {
                                            if(train1.sleeper_coach_mb[i][j]==0)
                                            {
                                                train1.sleeper_coach_mb[i][j]=1;
                                                char temp_str1[100];
                                                if(i%2==1) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+4);
                                                else if(i%2==0) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+7);
                                                strcat(response_to_agent, temp_str1);
                                                mb_req--;
                                            }
                                    }

                                    for(j=1;j<=18 && lb_req>0 && (train1.lb ==1) ;j++)
                                    {
                                            if(train1.sleeper_coach_lb[i][j]==0)
                                            {
                                                train1.sleeper_coach_lb[i][j]=1;
                                                char temp_str1[100];
                                                if(i%2==1) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+5);
                                                else if(i%2==0) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+8);
                                                strcat(response_to_agent, temp_str1);
                                                lb_req--;
                                            }
                                    }

                                    int seats_rem = su_req+sl_req+ub_req+mb_req+lb_req;
                                    for(int seat_no=1;seat_no<=72 && seats_rem>0 ;seat_no++)
                                    {
                                        if(seat_no%8==1)
                                        {
                                            //SU
                                            if(train1.sleeper_coach_su[i][(seat_no/8)+1]==0)
                                            {
                                                train1.sleeper_coach_su[i][(seat_no/8)+1]=1;

                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);

                                                seats_rem--; 
                                                //su_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==2)
                                        {
                                            //SL
                                            if(train1.sleeper_coach_sl[i][(seat_no/8)+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_sl[i][(seat_no/8)+1]=1;
                                                seats_rem--; 
                                                //sl_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==3)
                                        {
                                            //UB
                                            if(train1.sleeper_coach_ub[i][((seat_no-3)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_ub[i][((seat_no-3)/8)*2+1]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==6)
                                        {
                                            //UB
                                            if(train1.sleeper_coach_ub[i][((seat_no-6)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_ub[i][((seat_no-6)/8)*2+2]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==4)
                                        {
                                            //MB
                                            if(train1.sleeper_coach_mb[i][((seat_no-4)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_mb[i][((seat_no-4)/8)*2+1]=1;
                                                seats_rem--; 
                                                //mb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==7)
                                        {
                                            //MB
                                            if(train1.sleeper_coach_mb[i][((seat_no-7)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_mb[i][((seat_no-7)/8)*2+2]=1;
                                                seats_rem--;
                                                //mb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==5)
                                        {
                                            //LB
                                            if(train1.sleeper_coach_lb[i][((seat_no-5)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_lb[i][((seat_no-5)/8)*2+1]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==0)
                                        {
                                            //LB
                                            if(train1.sleeper_coach_lb[i][((seat_no-8)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_lb[i][((seat_no-8)/8)*2+2]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                    }


                                }                           
                            }


                            for(i=1;i<coach_found && (su_req||sl_req||ub_req||mb_req||lb_req) ;i++)
                            {
                                for(j=1;j<=9 && su_req>0 && (train1.su ==1) ;j++)
                                {
                                        if(train1.sleeper_coach_su[i][j]==0)
                                        {
                                            train1.sleeper_coach_su[i][j]=1;
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,8*(j-1)+1);
                                            strcat(response_to_agent, temp_str1);
                                            su_req--;
                                        }
                                }

                                for(j=1;j<=9 && sl_req>0 && (train1.sl ==1) ;j++)
                                {
                                        if(train1.sleeper_coach_sl[i][j]==0)
                                        {
                                            train1.sleeper_coach_sl[i][j]=1;
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,8*(j-1)+2);
                                            strcat(response_to_agent, temp_str1);
                                            sl_req--;
                                        }
                                }

                                for(j=1;j<=18 && ub_req>0 && (train1.ub ==1) ;j++)
                                {
                                        if(train1.sleeper_coach_ub[i][j]==0)
                                        {
                                            train1.sleeper_coach_ub[i][j]=1;
                                            char temp_str1[100];
                                            if(i%2==1) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+3);
                                            else if(i%2==0) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+6);
                                            strcat(response_to_agent, temp_str1);
                                            ub_req--;
                                        }
                                }

                                for(j=1;j<=18 && mb_req>0 && (train1.mb ==1) ;j++)
                                {
                                        if(train1.sleeper_coach_mb[i][j]==0)
                                        {
                                            train1.sleeper_coach_mb[i][j]=1;
                                            char temp_str1[100];
                                            if(i%2==1) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+4);
                                            else if(i%2==0) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+7);
                                            strcat(response_to_agent, temp_str1);
                                            mb_req--;
                                        }
                                }

                                for(j=1;j<=18 && lb_req>0 && (train1.lb ==1) ;j++)
                                {
                                        if(train1.sleeper_coach_lb[i][j]==0)
                                        {
                                            train1.sleeper_coach_lb[i][j]=1;
                                            char temp_str1[100];
                                            if(i%2==1) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+5);
                                            else if(i%2==0) sprintf(temp_str1,",S%d-%d",i,8*((j-1)/2)+8);
                                            strcat(response_to_agent, temp_str1);
                                            lb_req--;
                                        }
                                }    


                                int seats_rem = su_req+sl_req+ub_req+mb_req+lb_req;     
                                    for(int seat_no=1;seat_no<=72 && seats_rem>0 ;seat_no++)
                                    {
                                        if(seat_no%8==1)
                                        {
                                            //SU
                                            if(train1.sleeper_coach_su[i][(seat_no/8)+1]==0)
                                            {
                                                train1.sleeper_coach_su[i][(seat_no/8)+1]=1;

                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);

                                                seats_rem--; 
                                                //su_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==2)
                                        {
                                            //SL
                                            if(train1.sleeper_coach_sl[i][(seat_no/8)+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_sl[i][(seat_no/8)+1]=1;
                                                seats_rem--; 
                                                //sl_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==3)
                                        {
                                            //UB
                                            if(train1.sleeper_coach_ub[i][((seat_no-3)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_ub[i][((seat_no-3)/8)*2+1]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==6)
                                        {
                                            //UB
                                            if(train1.sleeper_coach_ub[i][((seat_no-6)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_ub[i][((seat_no-6)/8)*2+2]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==4)
                                        {
                                            //MB
                                            if(train1.sleeper_coach_mb[i][((seat_no-4)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_mb[i][((seat_no-4)/8)*2+1]=1;
                                                seats_rem--; 
                                                //mb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==7)
                                        {
                                            //MB
                                            if(train1.sleeper_coach_mb[i][((seat_no-7)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_mb[i][((seat_no-7)/8)*2+2]=1;
                                                seats_rem--;
                                                //mb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==5)
                                        {
                                            //LB
                                            if(train1.sleeper_coach_lb[i][((seat_no-5)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_lb[i][((seat_no-5)/8)*2+1]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%8==0)
                                        {
                                            //LB
                                            if(train1.sleeper_coach_lb[i][((seat_no-8)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.sleeper_coach_lb[i][((seat_no-8)/8)*2+2]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                    }
                            }
                        //else ends here
                        }                     
                    //sleeper part ends    
                    }
                }
                else if(strcmp(req_list[i].train,"12301")==0)
                {
                    if(strcmp(req_list[i].coach_type,"AC")==0)
                    {
                        int coach_found = 0;

                        //if((coach_found = is_suitable(train2.ac_coach_su,train2.ac_coach_sl,train2.ac_coach_ub,train2.ac_coach_mb,train2.ac_coach_lb,su_req,sl_req,ub_req,mb_req,lb_req,train2.ac_count))!=(-1))
                        if((coach_found = is_suitable(train2,su_req,sl_req,ub_req,mb_req,lb_req,'a'))!=(-1))
                        {
                            for(i=1;i<=9 && su_req>0 && (train2.su ==1) ;i++)
                            {
                                    if(train2.ac_coach_su[coach_found][i]==0)
                                    {
                                        train2.ac_coach_su[coach_found][i]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",A%d-%d",coach_found,6*(i-1)+1);
                                        strcat(response_to_agent, temp_str1);
                                        su_req--;
                                    }
                            }

                            for(i=1;i<=9 && sl_req>0 && (train2.sl ==1) ;i++)
                            {
                                    if(train2.ac_coach_sl[coach_found][i]==0)
                                    {
                                        train2.ac_coach_sl[coach_found][i]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",A%d-%d",coach_found,6*(i-1)+2);
                                        strcat(response_to_agent, temp_str1);
                                        sl_req--;
                                    }
                            }

                            for(i=1;i<=18 && ub_req>0 && (train2.ub ==1) ;i++)
                            {
                                    if(train2.ac_coach_ub[coach_found][i]==0)
                                    {
                                        train2.ac_coach_ub[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,6*((i-1)/2)+3);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,6*((i-1)/2)+5);
                                        strcat(response_to_agent, temp_str1);
                                        ub_req--;
                                    }
                            }

                            /*(for(i=1;i<=9 && mb_req>0 && (train2.mb ==1) ;i++)
                            {
                                    if(train2.ac_coach_mb[coach_found][i]==0)
                                    {
                                        train2.ac_coach_mb[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+7);
                                        strcat(response_to_agent, temp_str1);
                                        mb_req--;
                                    }
                            }*/

                            for(i=1;i<=18 && lb_req>0 && (train2.lb ==1) ;i++)
                            {
                                    if(train2.ac_coach_lb[coach_found][i]==0)
                                    {
                                        train2.ac_coach_lb[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,6*((i-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,6*((i-1)/2)+6);
                                        strcat(response_to_agent, temp_str1);
                                        lb_req--;
                                    }
                            }
                        }
                        else
                        {
                            //coach_found = best_poss_coach(train2.ac_coach_su,train2.ac_coach_sl,train2.ac_coach_ub,train2.ac_coach_mb,train2.ac_coach_lb,su_req,sl_req,ub_req,mb_req,lb_req,train2.ac_count);
                            coach_found = best_poss_coach(train2,su_req,sl_req,ub_req,mb_req,lb_req,'a');

                            for(i=1;i<=9 && su_req>0 && (train2.su ==1) ;i++)
                            {
                                    if(train2.ac_coach_su[coach_found][i]==0)
                                    {
                                        train2.ac_coach_su[coach_found][i]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",A%d-%d",coach_found,6*(i-1)+1);
                                        strcat(response_to_agent, temp_str1);
                                        su_req--;
                                    }
                            }

                            for(i=1;i<=9 && sl_req>0 && (train2.sl ==1) ;i++)
                            {
                                    if(train2.ac_coach_sl[coach_found][i]==0)
                                    {
                                        train2.ac_coach_sl[coach_found][i]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",A%d-%d",coach_found,6*(i-1)+2);
                                        strcat(response_to_agent, temp_str1);
                                        sl_req--;
                                    }
                            }

                            for(i=1;i<=18 && ub_req>0 && (train2.ub ==1) ;i++)
                            {
                                    if(train2.ac_coach_ub[coach_found][i]==0)
                                    {
                                        train2.ac_coach_ub[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,6*((i-1)/2)+3);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,6*((i-1)/2)+5);
                                        strcat(response_to_agent, temp_str1);
                                        ub_req--;
                                    }
                            }

                            /*for(i=1;i<=9 && mb_req>0 && (train2.mb ==1) ;i++)
                            {
                                    if(train2.ac_coach_mb[coach_found][i]==0)
                                    {
                                        train2.ac_coach_mb[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,8*((i-1)/2)+7);
                                        strcat(response_to_agent, temp_str1);
                                        mb_req--;
                                    }
                            }*/

                            for(i=1;i<=18 && lb_req>0 && (train2.lb ==1) ;i++)
                            {
                                    if(train2.ac_coach_lb[coach_found][i]==0)
                                    {
                                        train2.ac_coach_lb[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",A%d-%d",coach_found,6*((i-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",A%d-%d",coach_found,6*((i-1)/2)+6);
                                        strcat(response_to_agent, temp_str1);
                                        lb_req--;
                                    }
                            }


                            for(i=coach_found;i<=train2.ac_count && (su_req||sl_req||ub_req||mb_req||lb_req) ;i++)
                            {
                                if(i==coach_found)
                                {


                                    int seats_rem = su_req+sl_req+ub_req+mb_req+lb_req;
                                    for(int seat_no=1;seat_no<=54 && seats_rem>0 ;seat_no++)
                                    {
                                        if(seat_no%6==1)
                                        {
                                            //SU
                                            if(train2.ac_coach_su[i][(seat_no/6)+1]==0)
                                            {
                                                train2.ac_coach_su[i][(seat_no/6)+1]=1;

                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);

                                                seats_rem--; 
                                                //su_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%6==2)
                                        {
                                            //SL
                                            if(train2.ac_coach_sl[i][(seat_no/6)+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.ac_coach_sl[i][(seat_no/6)+1]=1;
                                                seats_rem--; 
                                                //sl_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%6==3)
                                        {
                                            //UB
                                            if(train2.ac_coach_ub[i][((seat_no-3)/6)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.ac_coach_ub[i][((seat_no-3)/6)*2+1]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%6==5)
                                        {
                                            //UB
                                            if(train2.ac_coach_ub[i][((seat_no-5)/6)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.ac_coach_ub[i][((seat_no-5)/6)*2+2]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        /*else if(seat_no%8==4)
                                        {
                                            //MB
                                            if(train1.ac_coach_mb[i][((seat_no-4)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_mb[i][((seat_no-4)/8)*2+1]=1;
                                                seats_rem--; 
                                                mb_req--;
                                            }
                                        }
                                        else if(seat_no%8==7)
                                        {
                                            //MB
                                            if(train1.ac_coach_mb[i][((seat_no-7)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_mb[i][((seat_no-7)/8)*2+2]=1;
                                                seats_rem--;
                                                mb_req--;
                                            }
                                        }*/
                                        else if(seat_no%6==4)
                                        {
                                            //LB
                                            if(train2.ac_coach_lb[i][((seat_no-4)/6)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.ac_coach_lb[i][((seat_no-4)/6)*2+1]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%6==0)
                                        {
                                            //LB
                                            if(train2.ac_coach_lb[i][((seat_no-6)/6)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.ac_coach_lb[i][((seat_no-6)/6)*2+2]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                    } 

                                }
                                else
                                {
                                    for(j=1;j<=9 && su_req>0 && (train2.su ==1) ;j++)
                                    {
                                            if(train2.ac_coach_su[i][j]==0)
                                            {
                                                train2.ac_coach_su[i][j]=1;
                                                char temp_str1[100];
                                                sprintf(temp_str1,",A%d-%d",i,6*(j-1)+1);
                                                strcat(response_to_agent, temp_str1);
                                                su_req--;
                                            }
                                    }

                                    for(j=1;j<=9 && sl_req>0 && (train2.sl ==1) ;j++)
                                    {
                                            if(train2.ac_coach_sl[i][j]==0)
                                            {
                                                train2.ac_coach_sl[i][j]=1;
                                                char temp_str1[100];
                                                sprintf(temp_str1,",A%d-%d",i,6*(j-1)+2);
                                                strcat(response_to_agent, temp_str1);
                                                sl_req--;
                                            }
                                    }

                                    for(j=1;j<=18 && ub_req>0 && (train2.ub ==1) ;j++)
                                    {
                                            if(train2.ac_coach_ub[i][j]==0)
                                            {
                                                train2.ac_coach_ub[i][j]=1;
                                                char temp_str1[100];
                                                if(i%2==1) sprintf(temp_str1,",A%d-%d",i,6*((j-1)/2)+3);
                                                else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,6*((j-1)/2)+5);
                                                strcat(response_to_agent, temp_str1);
                                                ub_req--;
                                            }
                                    }

                                    /*for(j=1;j<=9 && mb_req>0 && (train2.mb ==1) ;j++)
                                    {
                                            if(train2.ac_coach_mb[i][j]==0)
                                            {
                                                train2.ac_coach_mb[i][j]=1;
                                                char temp_str1[100];
                                                if(i%2==1) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+4);
                                                else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,8*((j-1)/2)+7);
                                                strcat(response_to_agent, temp_str1);
                                                mb_req--;
                                            }
                                    }*/

                                    for(j=1;j<=18 && lb_req>0 && (train2.lb ==1) ;j++)
                                    {
                                            if(train2.ac_coach_lb[i][j]==0)
                                            {
                                                train2.ac_coach_lb[i][j]=1;
                                                char temp_str1[100];
                                                if(i%2==1) sprintf(temp_str1,",A%d-%d",i,6*((j-1)/2)+4);
                                                else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,6*((j-1)/2)+6);
                                                strcat(response_to_agent, temp_str1);
                                                lb_req--;
                                            }
                                    } 

                                    int seats_rem = su_req+sl_req+ub_req+mb_req+lb_req;
                                    for(int seat_no=1;seat_no<=54 && seats_rem>0 ;seat_no++)
                                    {
                                        if(seat_no%6==1)
                                        {
                                            //SU
                                            if(train2.ac_coach_su[i][(seat_no/6)+1]==0)
                                            {
                                                train2.ac_coach_su[i][(seat_no/6)+1]=1;

                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);

                                                seats_rem--; 
                                                //su_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%6==2)
                                        {
                                            //SL
                                            if(train2.ac_coach_sl[i][(seat_no/6)+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.ac_coach_sl[i][(seat_no/6)+1]=1;
                                                seats_rem--; 
                                                //sl_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%6==3)
                                        {
                                            //UB
                                            if(train2.ac_coach_ub[i][((seat_no-3)/6)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.ac_coach_ub[i][((seat_no-3)/6)*2+1]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%6==5)
                                        {
                                            //UB
                                            if(train2.ac_coach_ub[i][((seat_no-5)/6)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.ac_coach_ub[i][((seat_no-5)/6)*2+2]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        /*else if(seat_no%8==4)
                                        {
                                            //MB
                                            if(train1.ac_coach_mb[i][((seat_no-4)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_mb[i][((seat_no-4)/8)*2+1]=1;
                                                seats_rem--; 
                                                mb_req--;
                                            }
                                        }
                                        else if(seat_no%8==7)
                                        {
                                            //MB
                                            if(train1.ac_coach_mb[i][((seat_no-7)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_mb[i][((seat_no-7)/8)*2+2]=1;
                                                seats_rem--;
                                                mb_req--;
                                            }
                                        }*/
                                        else if(seat_no%6==4)
                                        {
                                            //LB
                                            if(train2.ac_coach_lb[i][((seat_no-4)/6)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.ac_coach_lb[i][((seat_no-4)/6)*2+1]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%6==0)
                                        {
                                            //LB
                                            if(train2.ac_coach_lb[i][((seat_no-6)/6)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.ac_coach_lb[i][((seat_no-6)/6)*2+2]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                    }

                                }                           
                            }



                            for(i=1;i<coach_found && (su_req||sl_req||ub_req||mb_req||lb_req) ;i++)
                            {
                                for(j=1;j<=9 && su_req>0 && (train2.su ==1) ;j++)
                                {
                                        if(train2.ac_coach_su[i][j]==0)
                                        {
                                            train2.ac_coach_su[i][j]=1;
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,6*(j-1)+1);
                                            strcat(response_to_agent, temp_str1);
                                            su_req--;
                                        }
                                }

                                for(j=1;j<=9 && sl_req>0 && (train2.sl ==1) ;j++)
                                {
                                        if(train2.ac_coach_sl[i][j]==0)
                                        {
                                            train2.ac_coach_sl[i][j]=1;
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,6*(j-1)+2);
                                            strcat(response_to_agent, temp_str1);
                                            sl_req--;
                                        }
                                }

                                for(j=1;j<=18 && ub_req>0 && (train2.ub ==1) ;j++)
                                {
                                        if(train2.ac_coach_ub[i][j]==0)
                                        {
                                            train2.ac_coach_ub[i][j]=1;
                                            char temp_str1[100];
                                            if(i%2==1) sprintf(temp_str1,",A%d-%d",i,6*((j-1)/2)+3);
                                            else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,6*((j-1)/2)+5);
                                            strcat(response_to_agent, temp_str1);
                                            ub_req--;
                                        }
                                }

                                /*
                                for(j=1;j<=18 && mb_req>0 && (train2.mb ==1) ;j++)
                                {
                                        if(train2.ac_coach_mb[i][j]==0)
                                        {
                                            train2.ac_coach_mb[i][j]=1;
                                            char temp_str1[100];
                                            if(i%2==1) sprintf(temp_str1,",A%d-%d",i,6*((j-1)/2)+4);
                                            else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,6*((j-1)/2)+7);
                                            strcat(response_to_agent, temp_str1);
                                            mb_req--;
                                        }
                                }
                                */

                                for(j=1;j<=18 && lb_req>0 && (train2.lb ==1) ;j++)
                                {
                                        if(train2.ac_coach_lb[i][j]==0)
                                        {
                                            train2.ac_coach_lb[i][j]=1;
                                            char temp_str1[100];
                                            if(i%2==1) sprintf(temp_str1,",A%d-%d",i,6*((j-1)/2)+4);
                                            else if(i%2==0) sprintf(temp_str1,",A%d-%d",i,6*((j-1)/2)+6);
                                            strcat(response_to_agent, temp_str1);
                                            lb_req--;
                                        }
                                }    


                                int seats_rem = su_req+sl_req+ub_req+mb_req+lb_req;     
                                    for(int seat_no=1;seat_no<=54 && seats_rem>0 ;seat_no++)
                                    {
                                        if(seat_no%6==1)
                                        {
                                            //SU
                                            if(train2.ac_coach_su[i][(seat_no/6)+1]==0)
                                            {
                                                train2.ac_coach_su[i][(seat_no/6)+1]=1;

                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);

                                                seats_rem--; 
                                                //su_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%6==2)
                                        {
                                            //SL
                                            if(train2.ac_coach_sl[i][(seat_no/6)+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.ac_coach_sl[i][(seat_no/6)+1]=1;
                                                seats_rem--; 
                                                //sl_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%6==3)
                                        {
                                            //UB
                                            if(train2.ac_coach_ub[i][((seat_no-3)/6)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.ac_coach_ub[i][((seat_no-3)/6)*2+1]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%6==5)
                                        {
                                            //UB
                                            if(train2.ac_coach_ub[i][((seat_no-5)/6)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.ac_coach_ub[i][((seat_no-5)/6)*2+2]=1;
                                                seats_rem--; 
                                                //ub_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        /*else if(seat_no%8==4)
                                        {
                                            //MB
                                            if(train1.ac_coach_mb[i][((seat_no-4)/8)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_mb[i][((seat_no-4)/8)*2+1]=1;
                                                seats_rem--; 
                                                mb_req--;
                                            }
                                        }
                                        else if(seat_no%8==7)
                                        {
                                            //MB
                                            if(train1.ac_coach_mb[i][((seat_no-7)/8)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train1.ac_coach_mb[i][((seat_no-7)/8)*2+2]=1;
                                                seats_rem--;
                                                mb_req--;
                                            }
                                        }*/
                                        else if(seat_no%6==4)
                                        {
                                            //LB
                                            if(train2.ac_coach_lb[i][((seat_no-4)/6)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.ac_coach_lb[i][((seat_no-4)/6)*2+1]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                        else if(seat_no%6==0)
                                        {
                                            //LB
                                            if(train2.ac_coach_lb[i][((seat_no-6)/6)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.ac_coach_lb[i][((seat_no-6)/6)*2+2]=1;
                                                seats_rem--; 
                                                //lb_req--;
                                                if(su_req>0) su_req--;
                                                else if(sl_req>0) sl_req--;
                                                else if(ub_req>0) ub_req--;
                                                else if(mb_req>0) mb_req--;
                                                else if(lb_req>0) lb_req--;
                                            }
                                        }
                                    }
                            }












                        //else ends here
                        }
                        //ac part ends here
                    }
                    else if(strcmp(req_list[i].coach_type,"Sleeper")==0)
                    {
                        strcat(response_to_agent, ",Sorry! no berths available!\n");

                        cout<<"Generated resp - > "<<response_to_agent;

                        int rst = send (origin_of_msg, response_to_agent, strlen(response_to_agent), 0);
                        if (rst == -1)
                        {
                            //cerr<<"Hello1\n";
                            perror ("4. Server: Send failed");
                        }
                        else 
                        {
                            //cerr<<"rst -> "<<rst<<"\n";
                            printf ("S2C message sent successfully.\n");
                            fflush(stdout);
                        }
                            //goto end_part;
                            continue;                   

                        int coach_found = 0;
                        //if((coach_found = is_suitable(train2.sleeper_coach_su,train2.sleeper_coach_sl,train2.sleeper_coach_ub,train2.sleeper_coach_mb,train2.sleeper_coach_lb,su_req,sl_req,ub_req,mb_req,lb_req,train2.sleeper_count))!=(-1))
                        if((coach_found = is_suitable(train2,su_req,sl_req,ub_req,mb_req,lb_req,'s'))!=(-1))
                        {
                            for(i=1;i<=9 && su_req>0 && (train2.su ==1) ;i++)
                            {
                                    if(train2.sleeper_coach_su[coach_found][i]==0)
                                    {
                                        train2.sleeper_coach_su[coach_found][i]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",S%d-%d",coach_found,6*(i-1)+1);
                                        strcat(response_to_agent, temp_str1);
                                        su_req--;
                                    }
                            }

                            for(i=1;i<=9 && sl_req>0 && (train2.sl ==1) ;i++)
                            {
                                    if(train2.sleeper_coach_sl[coach_found][i]==0)
                                    {
                                        train2.sleeper_coach_sl[coach_found][i]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",S%d-%d",coach_found,6*(i-1)+2);
                                        strcat(response_to_agent, temp_str1);
                                        sl_req--;
                                    }
                            }

                            for(i=1;i<=18 && ub_req>0 && (train2.ub ==1) ;i++)
                            {
                                    if(train2.sleeper_coach_ub[coach_found][i]==0)
                                    {
                                        train2.sleeper_coach_ub[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,6*((i-1)/2)+3);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,6*((i-1)/2)+5);
                                        strcat(response_to_agent, temp_str1);
                                        ub_req--;
                                    }
                            }

                            /*for(i=1;i<=9 && mb_req>0 && (train2.mb ==1) ;i++)
                            {
                                    if(train2.sleeper_coach_mb[coach_found][i]==0)
                                    {
                                        train2.sleeper_coach_mb[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+7);
                                        strcat(response_to_agent, temp_str1);
                                        mb_req--;
                                    }
                            }*/

                            for(i=1;i<=18 && lb_req>0 && (train2.lb ==1) ;i++)
                            {
                                    if(train2.sleeper_coach_lb[coach_found][i]==0)
                                    {
                                        train2.sleeper_coach_lb[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,6*((i-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,6*((i-1)/2)+6);
                                        strcat(response_to_agent, temp_str1);
                                        lb_req--;
                                    }
                            }
                        }
                        else
                        {
                            //coach_found = best_poss_coach(train2.sleeper_coach_su,train2.sleeper_coach_sl,train2.sleeper_coach_ub,train2.sleeper_coach_mb,train2.sleeper_coach_lb,su_req,sl_req,ub_req,mb_req,lb_req,train2.sleeper_count);
                            coach_found = best_poss_coach(train2,su_req,sl_req,ub_req,mb_req,lb_req,'s');

                            for(i=1;i<=9 && su_req>0 && (train2.su ==1) ;i++)
                            {
                                    if(train2.sleeper_coach_su[coach_found][i]==0)
                                    {
                                        train2.sleeper_coach_su[coach_found][i]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",S%d-%d",coach_found,6*(i-1)+1);
                                        strcat(response_to_agent, temp_str1);
                                        su_req--;
                                    }
                            }

                            for(i=1;i<=9 && sl_req>0 && (train2.sl ==1) ;i++)
                            {
                                    if(train2.sleeper_coach_sl[coach_found][i]==0)
                                    {
                                        train2.sleeper_coach_sl[coach_found][i]=1;
                                        char temp_str1[100];
                                        sprintf(temp_str1,",S%d-%d",coach_found,6*(i-1)+2);
                                        strcat(response_to_agent, temp_str1);
                                        sl_req--;
                                    }
                            }

                            for(i=1;i<=18 && ub_req>0 && (train2.ub ==1) ;i++)
                            {
                                    if(train2.sleeper_coach_ub[coach_found][i]==0)
                                    {
                                        train2.sleeper_coach_ub[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,6*((i-1)/2)+3);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,6*((i-1)/2)+6);
                                        strcat(response_to_agent, temp_str1);
                                        ub_req--;
                                    }
                            }

                            /*for(i=1;i<=9 && mb_req>0 && (train2.mb ==1) ;i++)
                            {
                                    if(train2.sleeper_coach_mb[coach_found][i]==0)
                                    {
                                        train2.sleeper_coach_mb[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,8*((i-1)/2)+7);
                                        strcat(response_to_agent, temp_str1);
                                        mb_req--;
                                    }
                            }*/

                            for(i=1;i<=18 && lb_req>0 && (train2.lb ==1) ;i++)
                            {
                                    if(train2.sleeper_coach_lb[coach_found][i]==0)
                                    {
                                        train2.sleeper_coach_lb[coach_found][i]=1;
                                        char temp_str1[100];
                                        if(i%2==1) sprintf(temp_str1,",S%d-%d",coach_found,6*((i-1)/2)+4);
                                        else if(i%2==0) sprintf(temp_str1,",S%d-%d",coach_found,6*((i-1)/2)+6);
                                        strcat(response_to_agent, temp_str1);
                                        lb_req--;
                                    }
                            }


                            for(i=coach_found;i<=train2.sleeper_count && (su_req||sl_req||ub_req||mb_req||lb_req) ;i++)
                            {

                                if(i==coach_found)
                                {
                                    int seats_rem = su_req+sl_req+ub_req+mb_req+lb_req;
                                    for(int seat_no=1;seat_no<=54 && seats_rem>0 ;seat_no++)
                                    {
                                        if(seat_no%6==1)
                                        {
                                            //SU
                                            if(train2.sleeper_coach_su[i][(seat_no/6)+1]==0)
                                            {
                                                train2.sleeper_coach_su[i][(seat_no/6)+1]=1;

                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);

                                                seats_rem--; 
                                                su_req--;
                                            }
                                        }
                                        else if(seat_no%6==2)
                                        {
                                            //SL
                                            if(train2.sleeper_coach_sl[i][(seat_no/6)+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.sleeper_coach_sl[i][(seat_no/6)+1]=1;
                                                seats_rem--; 
                                                sl_req--;
                                            }
                                        }
                                        else if(seat_no%6==3)
                                        {
                                            //UB
                                            if(train2.sleeper_coach_ub[i][((seat_no-3)/6)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.sleeper_coach_ub[i][((seat_no-3)/6)*2+1]=1;
                                                seats_rem--; 
                                                ub_req--;
                                            }
                                        }
                                        else if(seat_no%6==5)
                                        {
                                            //UB
                                            if(train2.sleeper_coach_ub[i][((seat_no-5)/6)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.sleeper_coach_ub[i][((seat_no-5)/6)*2+2]=1;
                                                seats_rem--; 
                                                ub_req--;
                                            }
                                        }
                                        else if(seat_no%6==4)
                                        {
                                            //LB
                                            if(train2.sleeper_coach_lb[i][((seat_no-4)/6)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.sleeper_coach_lb[i][((seat_no-4)/6)*2+1]=1;
                                                seats_rem--; 
                                                lb_req--;
                                            }
                                        }
                                        else if(seat_no%6==0)
                                        {
                                            //LB
                                            if(train2.sleeper_coach_lb[i][((seat_no-6)/6)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",A%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.sleeper_coach_lb[i][((seat_no-6)/6)*2+2]=1;
                                                seats_rem--; 
                                                lb_req--;
                                            }
                                        }
                                    } 

                                }
                                else
                                {

                                    for(j=1;j<=9 && su_req>0 && (train2.su ==1) ;j++)
                                    {
                                            if(train2.sleeper_coach_su[i][j]==0)
                                            {
                                                train2.sleeper_coach_su[i][j]=1;
                                                char temp_str1[100];
                                                sprintf(temp_str1,",S%d-%d",i,6*(j-1)+1);
                                                strcat(response_to_agent, temp_str1);
                                                su_req--;
                                            }
                                    }

                                    for(j=1;j<=9 && sl_req>0 && (train2.sl ==1) ;j++)
                                    {
                                            if(train2.sleeper_coach_sl[i][j]==0)
                                            {
                                                train2.sleeper_coach_sl[i][j]=1;
                                                char temp_str1[100];
                                                sprintf(temp_str1,",S%d-%d",i,6*(j-1)+2);
                                                strcat(response_to_agent, temp_str1);
                                                sl_req--;
                                            }
                                    }

                                    for(j=1;j<=18 && ub_req>0 && (train2.ub ==1) ;j++)
                                    {
                                            if(train2.sleeper_coach_ub[i][j]==0)
                                            {
                                                train2.sleeper_coach_ub[i][j]=1;
                                                char temp_str1[100];
                                                if(i%2==1) sprintf(temp_str1,",S%d-%d",i,6*((j-1)/2)+3);
                                                else if(i%2==0) sprintf(temp_str1,",S%d-%d",i,6*((j-1)/2)+5);
                                                strcat(response_to_agent, temp_str1);
                                                ub_req--;
                                            }
                                    }

                                    for(j=1;j<=18 && lb_req>0 && (train2.lb ==1) ;j++)
                                    {
                                            if(train2.sleeper_coach_lb[i][j]==0)
                                            {
                                                train2.sleeper_coach_lb[i][j]=1;
                                                char temp_str1[100];
                                                if(i%2==1) sprintf(temp_str1,",S%d-%d",i,6*((j-1)/2)+4);
                                                else if(i%2==0) sprintf(temp_str1,",S%d-%d",i,6*((j-1)/2)+6);
                                                strcat(response_to_agent, temp_str1);
                                                lb_req--;
                                            }
                                    }                            

                                    int seats_rem = su_req+sl_req+ub_req+mb_req+lb_req;
                                    for(int seat_no=1;seat_no<=54 && seats_rem>0 ;seat_no++)
                                    {
                                        if(seat_no%6==1)
                                        {
                                            //SU
                                            if(train2.sleeper_coach_su[i][(seat_no/6)+1]==0)
                                            {
                                                train2.sleeper_coach_su[i][(seat_no/6)+1]=1;

                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);

                                                seats_rem--; 
                                                su_req--;
                                            }
                                        }
                                        else if(seat_no%6==2)
                                        {
                                            //SL
                                            if(train2.sleeper_coach_sl[i][(seat_no/6)+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.sleeper_coach_sl[i][(seat_no/6)+1]=1;
                                                seats_rem--; 
                                                sl_req--;
                                            }
                                        }
                                        else if(seat_no%6==3)
                                        {
                                            //UB
                                            if(train2.sleeper_coach_ub[i][((seat_no-3)/6)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.sleeper_coach_ub[i][((seat_no-3)/6)*2+1]=1;
                                                seats_rem--; 
                                                ub_req--;
                                            }
                                        }
                                        else if(seat_no%6==5)
                                        {
                                            //UB
                                            if(train2.sleeper_coach_ub[i][((seat_no-5)/6)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.sleeper_coach_ub[i][((seat_no-5)/6)*2+2]=1;
                                                seats_rem--; 
                                                ub_req--;
                                            }
                                        }
                                        else if(seat_no%6==4)
                                        {
                                            //LB
                                            if(train2.sleeper_coach_lb[i][((seat_no-4)/6)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.sleeper_coach_lb[i][((seat_no-4)/6)*2+1]=1;
                                                seats_rem--; 
                                                lb_req--;
                                            }
                                        }
                                        else if(seat_no%6==0)
                                        {
                                            //LB
                                            if(train2.sleeper_coach_lb[i][((seat_no-6)/6)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.sleeper_coach_lb[i][((seat_no-6)/6)*2+2]=1;
                                                seats_rem--; 
                                                lb_req--;
                                            }
                                        }
                                    }



                                }

                            }



                            for(i=1;i<coach_found && (su_req||sl_req||ub_req||mb_req||lb_req) ;i++)
                            {
                                for(j=1;j<=9 && su_req>0 && (train2.su ==1) ;j++)
                                {
                                        if(train2.sleeper_coach_su[i][j]==0)
                                        {
                                            train2.sleeper_coach_su[i][j]=1;
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,6*(j-1)+1);
                                            strcat(response_to_agent, temp_str1);
                                            su_req--;
                                        }
                                }

                                for(j=1;j<=9 && sl_req>0 && (train2.sl ==1) ;j++)
                                {
                                        if(train2.sleeper_coach_sl[i][j]==0)
                                        {
                                            train2.sleeper_coach_sl[i][j]=1;
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,6*(j-1)+2);
                                            strcat(response_to_agent, temp_str1);
                                            sl_req--;
                                        }
                                }

                                for(j=1;j<=18 && ub_req>0 && (train2.ub ==1) ;j++)
                                {
                                        if(train2.sleeper_coach_ub[i][j]==0)
                                        {
                                            train2.sleeper_coach_ub[i][j]=1;
                                            char temp_str1[100];
                                            if(i%2==1) sprintf(temp_str1,",S%d-%d",i,6*((j-1)/2)+3);
                                            else if(i%2==0) sprintf(temp_str1,",S%d-%d",i,6*((j-1)/2)+5);
                                            strcat(response_to_agent, temp_str1);
                                            ub_req--;
                                        }
                                }

                                for(j=1;j<=18 && lb_req>0 && (train2.lb ==1) ;j++)
                                {
                                        if(train2.sleeper_coach_lb[i][j]==0)
                                        {
                                            train2.sleeper_coach_lb[i][j]=1;
                                            char temp_str1[100];
                                            if(i%2==1) sprintf(temp_str1,",S%d-%d",i,6*((j-1)/2)+4);
                                            else if(i%2==0) sprintf(temp_str1,",S%d-%d",i,6*((j-1)/2)+6);
                                            strcat(response_to_agent, temp_str1);
                                            lb_req--;
                                        }
                                }    


                                    int seats_rem = su_req+sl_req+ub_req+mb_req+lb_req;     
                                    for(int seat_no=1;seat_no<=54 && seats_rem>0 ;seat_no++)
                                    {
                                        if(seat_no%6==1)
                                        {
                                            //SU
                                            if(train2.sleeper_coach_su[i][(seat_no/6)+1]==0)
                                            {
                                                train2.sleeper_coach_su[i][(seat_no/6)+1]=1;

                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);

                                                seats_rem--; 
                                                su_req--;
                                            }
                                        }
                                        else if(seat_no%6==2)
                                        {
                                            //SL
                                            if(train2.sleeper_coach_sl[i][(seat_no/6)+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.sleeper_coach_sl[i][(seat_no/6)+1]=1;
                                                seats_rem--; 
                                                sl_req--;
                                            }
                                        }
                                        else if(seat_no%6==3)
                                        {
                                            //UB
                                            if(train2.sleeper_coach_ub[i][((seat_no-3)/6)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.sleeper_coach_ub[i][((seat_no-3)/6)*2+1]=1;
                                                seats_rem--; 
                                                ub_req--;
                                            }
                                        }
                                        else if(seat_no%6==5)
                                        {
                                            //UB
                                            if(train2.sleeper_coach_ub[i][((seat_no-5)/6)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.sleeper_coach_ub[i][((seat_no-5)/6)*2+2]=1;
                                                seats_rem--; 
                                                ub_req--;
                                            }
                                        }
                                        else if(seat_no%6==4)
                                        {
                                            //LB
                                            if(train2.sleeper_coach_lb[i][((seat_no-4)/6)*2+1]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.sleeper_coach_lb[i][((seat_no-4)/6)*2+1]=1;
                                                seats_rem--; 
                                                lb_req--;
                                            }
                                        }
                                        else if(seat_no%6==0)
                                        {
                                            //LB
                                            if(train2.sleeper_coach_lb[i][((seat_no-6)/6)*2+2]==0)
                                            {
                                            char temp_str1[100];
                                            sprintf(temp_str1,",S%d-%d",i,seat_no);
                                            strcat(response_to_agent, temp_str1);
                                                train2.sleeper_coach_lb[i][((seat_no-6)/6)*2+2]=1;
                                                seats_rem--; 
                                                lb_req--;
                                            }
                                        }
                                    }
                            }





                        //else ends here
                        }                     
                    //sleeper part ends    
                    }    
                }
            }

            strcat(response_to_agent, "\n");
            

            cout<<"Final Generated resp - > "<<response_to_agent;

            cout<<"OR - > "<<origin_of_msg<<"\n";
            int rst = send (origin_of_msg, response_to_agent, strlen(response_to_agent), 0);
            if (rst == -1)
            {
                //cerr<<"Hello1\n";
                perror ("4. Server: Send failed");
            }
            else 
            {
                //cerr<<"rst -> "<<rst<<"\n";
                printf ("S2C message sent successfully.\n");
                //fflush(stdout);
            }
            i=k;

        }
	}
	return 0;
}
