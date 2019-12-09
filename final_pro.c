#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_LENGTH 100
#define K 35
//


int iniit_cluster (char words[][MAX_LENGTH], int * rep, int * clstr_no,  int num_words, int rep_num);
void new_cluster_center (char words[][MAX_LENGTH], int * rep, int * clstr_no,  int num_words, int rep_num);
void makeHistogram (char rep[][MAX_LENGTH]);
int min (int i, int j);
int max (int i, int j);
void test (int histoArray[][K],char rep[][MAX_LENGTH]);
int ReturnDistance(char *str1,char * str2,int i ,int j);
void makeUseful (char* temp);
void random_ar (int * arr,int num, int max) ;
void computeKmeans ();
void makeLower (char * sent);
void makeHistogram (char rep[][MAX_LENGTH]);
int histo_sprt (char * sentacnce, char * rep_word  );

int min (int i, int j) {
return (i<j)?i:j;
}
int max (int i, int j) {
return (i>j)?i:j;
}

int ReturnDistance(char *str1,char * str2,int i ,int j) {
if (min(i,j)==-1) {
return max(i,j)+1;
}
if (str1[i]==str2[j]) {
return  ReturnDistance(str1,str2,i-1,j-1);
} else {
int delfrom1 = ReturnDistance(str1,str2,i-1,j)+1;
int delfrom2 = ReturnDistance(str1,str2,i,j-1)+1;
int substitute = ReturnDistance(str1,str2,i-1,j-1)+1;
return min(min(delfrom2,delfrom1),substitute);
}
}

void makeUseful (char* temp) {
int len = strlen (temp);
int i;
for (i=0;i<=len;i++) {
if (temp[i]>=65 && temp[i]<=90) {
temp[i] = temp[i]+ 32;
}
if (temp[i]>=33 && temp[i]<=47) {
temp[i]='\0';
}
if (temp[i]=='?') {
temp[i]='\0';
}
}
}

void random_ar (int * arr,int num, int max) {
srand (time(NULL));
int seed = max/num-1;
int temp = rand()%seed+1;
arr[0]=rand()%seed;
int i;
for (i =1;i<num;i++) {
arr[i] = arr[i-1] + rand()%seed;
}
}
#define K_spam 14
#define K_ham 21
void computeKmeans () {
printf("Computing K Means\n");
char spam_wrd [350][MAX_LENGTH]; //14 14 different rand
char ham_wrd [700][MAX_LENGTH];  //28 different rand nos.
int spam_rep[K_spam];
int ham_rep[K_ham]; // Spam_wrd array representative words of ham cluster ka index contain karega
int spam_clstr_no[350]={0}; // Ye array jo bhi train word hai...Wo kis me hai ye conatcain karega.. Initially every in 0 th rep ke cluster me...
int ham_clstr_no[700]={0};
int combined_rep [K];
random_ar(spam_rep,K_spam,350);
random_ar(ham_rep,K_ham,700);


FILE * read =(FILE *) (fopen("out.txt","r"));
if (read==NULL) {
printf("File Not Found\n");
return;
}
int i;
for (i = 0;i<350;i++) {
fscanf (read,"%s",spam_wrd[i]);
}
fseek ( read ,-5700 , SEEK_END );
for (i = 0;i<700;i++) {
fscanf (read,"%s",ham_wrd[i]);
}
printf("Generating Representatives for Spam\n" );
int till = 1;
while (till!=0) {
till = iniit_cluster(spam_wrd, spam_rep, spam_clstr_no, 350, K_spam);
new_cluster_center(spam_wrd, spam_rep, spam_clstr_no, 350, K_spam);
}
printf("\n\n\tRepresentative words for Spam\n");
for (i = 0;i<K_spam;i++) {
printf("\t-%s\n",spam_wrd[spam_rep[i]] );
}
printf("\n\n\n");
printf("Generating Representatives for Ham\n" );
till = 1;
while (till!=0) {
till = iniit_cluster(ham_wrd, ham_rep, ham_clstr_no, 700, K_ham);
new_cluster_center(ham_wrd, ham_rep, ham_clstr_no, 700, K_ham);
}
printf("\n\n\tRepresentative words for Ham\n");
for (i = 0;i<K_ham;i++) {
printf("\t-%s\n",(ham_rep[i]!=(-1))?ham_wrd[ham_rep[i]]:"Empty Cluster" );
}
printf("\n\n\n");

fclose(read);


char representatives[K][MAX_LENGTH];
for (i =0;i<K_spam;i++) {
strcpy(representatives[i],spam_wrd[spam_rep[i]]);

}
char Empty[] = "EMPTY\0";
for (i =0;i<K_ham;i++) {
if (ham_rep[i]==(-1)) {
strcpy(representatives[i+K_spam],Empty);
continue;
}
strcpy(representatives[i+K_spam],ham_wrd[ham_rep[i]]);
}


makeHistogram(representatives);

}


void new_cluster_center (char words[][MAX_LENGTH], int * rep, int * clstr_no,  int num_words, int rep_num  ) {
printf("Deciding New Cluster Centers\n");
int i,j,k; //Loopping Variables
for (i =0;i<rep_num;i++) { // ith cluster ke liye
int min = 1000000;
int i_min = -1;
for (j = 0;(j<num_words);j++) {
if (clstr_no[j]!=i) {continue;}
int sum = 0;
for (k = 0;(k<num_words)  ;k++) {
if (clstr_no[k]!=i) {continue;}
int temp = ReturnDistance(words[k], words[j], strlen(words[k]),strlen(words[j]));
sum = sum +temp;
}
if (sum<min) {
min  = sum;
i_min = j; // j cluster center banega
}
}
rep[i]=i_min;
}
}
int iniit_cluster (char words[][MAX_LENGTH], int * rep, int * clstr_no,  int num_words, int rep_num ) { // returns 1 when cluster changed
printf("Clustering Words Into Clusters\n" );
int i,j; // Looping Varibles
int to_return = 0; // Returns whether cluster changed or not
for (i = 0;i<num_words;i++) {
int min = ReturnDistance(words[i],words[rep[0]],strlen(words[i]),strlen(words[rep[0]])) ;
int i_min = 0;     // Initalise
for (j =0;j<rep_num;j++) {
int temp = ReturnDistance(words[i],words[rep[j]],strlen(words[i]),strlen(words[rep[j]])) ;
if (temp<min) {
min = temp;
i_min = j;
}
}
if (clstr_no[i]!=i_min) {
to_return = 1;  //Yani cluster change ho raha hai kisi word ka
}
clstr_no[i] = i_min;
}
return to_return;
}

void makeLower (char * sent) {
int i =0;
while ((sent[i])!='\n') {
if (sent[i]>=65 &&sent[i]<=90) {
sent[i] = sent[i]+32;
}
if (sent[i]>=58 &&sent[i]<=64) {
sent[i]=' ';
}
if (sent[i]>=33 &&sent[i]<=47 )  {
sent[i]=' ';
}
i++;
}

}

int computeL1Distance  (int * array, int histogram[][K]) {
int i,j;
//printf("In compute L");
int min=1000000; int i_min =-1;
int num_max = 543;
for  (i =0;i<num_max;i++) {
int sum = 0;
for (j =0;j<K;j++) {
sum = sum + abs(array[j]-histogram[i][j]);
}
if (sum <min) {
min = sum;
i_min = i;
}
}

return i_min;

}


int histo_sprt (char * sentacnce, char * rep_word  ) {
char word[MAX_LENGTH];
int sum = 0;
if (strcmp(rep_word,"EMPTY")==0) {
return 0;
}
while (sscanf (sentacnce,"%s",word)>0) {
//printf("**%s\n",word );
if (strlen(word)>=8 || strlen(word)<=2) {
sentacnce = sentacnce + strlen(word)+1;continue;
}
if ((strcmp(word,"spam")==0) || (strcmp(word,"ham")==0)) {
return sum;
}  //Decide Later

int temp = ReturnDistance(word, rep_word, strlen(word), strlen(rep_word));
sum = sum + temp;
sentacnce = sentacnce + strlen(word)+1;
}
return sum;
}

void test (int histoArray[][K],char rep[][MAX_LENGTH]) {
printf("Test Initiated\n");
int i;

int spam=0;
int ham=0;

int correct = 0;

FILE * source = (FILE *)fopen ("input.txt","r");
if (source ==NULL) {
printf("File Not Found\n");
return;

}

char temp [10000];

int histo_msg[K];
int sentacnce_no = 0;
while (sentacnce_no<42) { // skipped
fgets (temp,9998,source);
sentacnce_no++;
//printf("%d %s\n",sentacnce_no,temp );
}
//printf("%s\n",temp );
while (sentacnce_no >=43 && sentacnce_no<=82) {
fgets (temp,9998,source);
makeLower(temp);
for (i =0;i<K;i++) {
int ret_val = histo_sprt(temp, rep[i]);
histo_msg[i] = ret_val;
}
int index = computeL1Distance (histo_msg,histoArray ); //
if (index <=42) {
spam++;
correct++;

}  else {
ham++;

}

sentacnce_no++;

}
while (sentacnce_no<543) { // skipped upto 4
fgets (temp,9998,source);
sentacnce_no++;
}
while (sentacnce_no<=1083) {
fgets (temp,9998,source);
makeLower(temp);
for (i =0;i<K;i++) {
int ret_val = histo_sprt(temp, rep[i]);
histo_msg[i] = ret_val;

}
int index = computeL1Distance (histo_msg,histoArray ); //
if (index <=42) {
spam++;

}  else {
ham++;
correct++;

}

sentacnce_no++;
}

printf("\n\n-----Stats-----\n");
printf("Total No. of clusters taken %d\nTotal No. of Cluster Center created from Spam Word =  %d\nTotal No. of Cluster Center created from Ham Word =  %d\n ",K,K_spam,K_ham);
printf("Total Messages Checked  = 543\n" );
int judged = (abs(spam-42)+abs(ham-501));
judged = judged/2; // Each error contributed to two
printf("Total correct ham/spam Found = %d out of %d\n", 543-judged,543);
printf("%f\n",((float)(543-judged)/543)*100);



}


void makeHistogram (char rep[][MAX_LENGTH]) {
printf("Generating Histogram...\n");
int i,j;
FILE * source = (FILE * )fopen("input.txt","r");
if (source==NULL) {
printf("File Not Found\n");
return;
}
int histoArray [543][K]={0}; // 0 se 500
char temp[10000];
int sentance_no=0;


while ((fgets(temp,9998,source)!=NULL) &&(sentance_no<42) ) { // 42 msgs
makeLower (temp);
for (i =0;i<K;i++) {
int ret_val = histo_sprt(temp, rep[i]);
histoArray[sentance_no][i] = ret_val;
}
sentance_no++;
}

for (i =0;i<42;i++) {   // Skip Next 42 Msgs
fgets(temp,9998,source);
}


while ((fgets(temp,9998,source)!=NULL) &&(sentance_no<543) ) {
makeLower (temp);
for (i =0;i<K;i++) {
histoArray[sentance_no][i] = histo_sprt (temp, rep[i]);
}
sentance_no++;
}

/*for (i =0;i<543;i++) {  // Uncomment this line to see Histogram

for (j = 0;j<K;j++) {
printf("%d ",histoArray[i][j]);
}
printf("\n");
}*/

printf("Histogram Generated...\n");
fclose(source);

test(histoArray,rep);
}


int main () {
FILE * file = (FILE *) fopen ("input.txt","r");
if (file==NULL) {
printf("File Not Found\n");
return 0;
}
FILE * out =(FILE * ) fopen ("out.txt","w");
char temp [MAX_LENGTH];
char final [MAX_LENGTH];
while ((fscanf(file,"%s",temp))!=EOF)  {
makeUseful(temp);
if (strlen(temp)<=2 || strlen(temp)>=9) {
continue;
}
if (strcmp(temp,"the") ==0 ) {
continue;
}
if (strcmp(temp,"are") ==0 ) {
continue;
}
fprintf(out,"%s\n",temp);
}
fclose (file);
fclose (out);
computeKmeans();
}
