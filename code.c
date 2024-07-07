#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

struct line {
    int * tags;
    int num_tags_stored;
    int * valid_bit;
    int to_be_replaced;
};

int findpower(int a,int b){
    int power=0;
    while(a>1){
        a=a/b;
        power++;
    }    
    return power;
}

void shiftLeft(int tags[],int location,int associativity){
    for (int i = location; i < associativity-1; i++){
        tags[i]=tags[i+1];
    }
}

void shiftleftCache(struct line Cache[],int location,int num_sets){
    struct line * one=NULL;
    struct line * two=NULL;
    for(int i=location;i<num_sets-1;i++){
        one=&Cache[i];
        two=&Cache[i+1];
        one->tags[0]=two->tags[0];
        one->valid_bit[0]=two->valid_bit[0];
    }
}


long long HexToDecimal(char hexString[8]){
    long long decimalValue=0;
    for(int index=7;index>=0;index--){
        if(hexString[index]>='0'&& hexString[index]<='9'){
            decimalValue=decimalValue+(hexString[index]-48)*pow(16,7-index);
        }
        else if(hexString[index]>='A'&& hexString[index]<='F'){
            decimalValue=decimalValue+(hexString[index]-55)*pow(16,7-index);
        }
        else if(hexString[index]>='a'&& hexString[index]<='f'){
            decimalValue=decimalValue+(hexString[index]-87)*pow(16,7-index);
        }
    }
    return decimalValue;
}

void DecimalToBinary(long long decimal,int binary[32]){
    for(int index=0;index<32;index++){
        binary[index]=decimal%2;
        decimal=decimal/2;
    }
}

int extract(int binary[32],int size,int end){
    int extractedpart=0;
    int position=end;
    int power=size-1;
    int index=0;
    while(index<size){
        extractedpart=extractedpart+binary[position]*pow(2,power);
        position--;
        index++;
        power--;
    }
    return extractedpart;
}

void Decode(char hexString[8],int binary[32]){ 
    long long decimalValue=HexToDecimal(hexString);
    DecimalToBinary(decimalValue,binary);
}

void Access(struct line Cache[],int binary[32],int bits_tag,int bits_setindex,int bits_offset,int mode,int associativity,int replacement,int write_policy){
    int tag,setindex;
    struct line * current=NULL;
    if(associativity!=0){
        tag=extract(binary,bits_tag,31);
        setindex=extract(binary,bits_setindex,bits_offset+bits_setindex-1);
    }
    else{
        tag=extract(binary,bits_tag+bits_setindex,31);
    }   
    if(associativity!=0){
        printf("Set: 0x%x, ",setindex);
        int hit=0;
        current=&Cache[setindex];
        for (int i = 0; i < current->num_tags_stored; i++){
            if(current->tags[i]==tag && current->valid_bit[i]==1){
                printf("Hit, ");
                if(replacement==2){
                    shiftLeft(current->tags,i,associativity);
                    current->tags[current->num_tags_stored-1]=tag;
                }
                hit++;
                break;
            }
        }
        if(hit==0 && write_policy==1 && mode==2){
            printf("Miss, ");
        }
        else if(hit==0){
            printf("Miss, ");
            if(current->num_tags_stored<associativity){
                current->tags[current->num_tags_stored]=tag;
                current->valid_bit[current->num_tags_stored]=1;
                current->num_tags_stored++;
            }
            else{
                /*Need to replace*/
                if(replacement==1 || replacement==2){
                    shiftLeft(current->tags,0,associativity);
                    current->tags[associativity-1]=tag;
                }
                else{
                    srand(time(0));
                    current->to_be_replaced=rand()%associativity;
                    current->tags[current->to_be_replaced]=tag;
                }
            }
        }
        printf("Tag: 0x%x\n",tag);
        
    }
    else if(associativity==0){
        int hit=0;
        int i=0;
        int num_sets=(int)pow(2,bits_setindex);
        printf("Set: 0x0, ");
        for (i = 0; i < num_sets; i++){
            current=&Cache[i];
            if(current->tags[0]==tag && current->valid_bit[0]==1){
                printf("Hit, ");
                hit++;
                if(replacement==2){
                    /*implement lru*/
                    shiftleftCache(Cache,i,num_sets);
                }
            }
            if(current->valid_bit[0]==0){
                break;
            }
        }
        if(hit==1 && replacement==2){
            if(i<num_sets){
                current=&Cache[i];
                current->tags[0]=tag;
                current->valid_bit[0]=1;
            }
            else{
                current=&Cache[num_sets-1];
                current->tags[0]=tag;
            }
        }
        if(hit==0 && write_policy==1 && mode==2){
            printf("Miss, ");
        }
        else if(hit==0){
            printf("Miss, ");
            if(i<num_sets){
                current=&Cache[i];
                current->tags[0]=tag;
                current->valid_bit[0]=1;
            }
            else{
                /*Need to replace*/
                if(replacement==1 || replacement==2){
                    shiftleftCache(Cache,0,num_sets);
                    current=&Cache[num_sets-1];
                    current->tags[0]=tag;
                }
                else if(replacement==3){
                    srand(time(0));
                    int to_be_replaced=rand()%num_sets;
                    current=&Cache[to_be_replaced];
                    current->tags[0]=tag;
                }
            }
        }
        printf("Tag: 0x%x\n",tag);
    }    
}


int main(){
    char filename1[100],filename2[100];
    printf("Give name of config file\n");
    scanf("%s",filename1);
    FILE * file1;
    file1=fopen(filename1,"r");
    if(file1==NULL){
        printf("Error opening file\n");
    }
    else{
        char line[100];
        int cache_size=0;
        int block_size=0;
        int associativity=0;
        int i=0;
        int replacement=0;/* FIFO-1 LRU-2 Random-3*/
        int write_policy=0;/*WT-1 WB-2*/
        int num_sets=0;
        int bits_offset=0;
        int bits_setindex=0;
        int bits_tag=0;
        while (fgets(line, sizeof(line), file1) != NULL) {
            i++;
            line[strcspn(line, "\n")] = '\0';
            if(i==1){
                cache_size=atoi(line);
            }
            else if (i==2){
                block_size=atoi(line);
            }
            else if (i==3){
                associativity=atoi(line);
            }
            else if(i==4){
                if(strncmp(line,"FIFO",4)==0){
                    replacement=1;
                }
                else if(strncmp(line,"LRU",3)==0){
                    replacement=2;
                }
                else if(strncmp(line,"Random",6)==0){
                    replacement=3;
                }
            }
            else if(i==5){
                if(strncmp(line,"WT",2)==0){
                    write_policy=1;
                }
                else if(strncmp(line,"WB",2)==0){
                    write_policy=2;
                }
            }
        }
        if(associativity!=0){
            num_sets=cache_size/(block_size*associativity);
        }
        else{
            num_sets=cache_size/block_size;
        }        
        bits_offset=findpower(block_size,2);
        bits_setindex=findpower(num_sets,2);
        bits_tag=32-bits_offset-bits_setindex;
        struct line Cache[num_sets];
        for (int i = 0; i < num_sets; i++){
            if(associativity!=0){
                Cache[i].tags=malloc((sizeof(int))*associativity);
                Cache[i].valid_bit=malloc(sizeof(int)*associativity);
            }
            else{
                Cache[i].tags=malloc(sizeof(int));
                Cache[i].valid_bit=malloc(sizeof(int));
            }
            Cache[i].num_tags_stored=0;
            Cache[i].to_be_replaced=0;
        }
        fclose(file1);
        printf("Give name of access file\n");
        scanf("%s",filename2);
        char * token;
        int access_type=0;/*1-Read,2-Write*/
        FILE * file2;
        file2=fopen(filename2,"r");
        if(file2==NULL){
            printf("File doesn't exist\n");
        }
        else{
            while (fgets(line, sizeof(line), file2) != NULL) {
                line[strcspn(line, "\n")] = '\0';
                token=strtok(line,": ");
                if(strncmp(token,"R",1)==0){
                    access_type=1;
                }
                else if(strncmp(token,"W",1)==0){
                    access_type=2;
                }
                token=strtok(NULL," ");
                int binary[32]={0};
                int length=strlen(token);
                char address[9];
                int num_zeros_to_add=10-length;
                if(length<10){
                    token=token+2;
                    for (int i = 0; i < 9; i++){
                        if(num_zeros_to_add>0){
                            address[i]='0';
                            num_zeros_to_add--;
                        }
                        else{
                            address[i]=*token;
                            token++;
                        }
                    }                
                }
                else{
                    strncpy(address,token+2,8);
                }
                address[8]='\0';
                printf("Address: 0x%s,",address);
                Decode(address,binary);
                Access(Cache,binary,bits_tag,bits_setindex,bits_offset,access_type,associativity,replacement,write_policy);
            }
        }        
    }
}