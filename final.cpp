#include <bits\stdc++.h>
#include <fstream>
using namespace std;

//choices
// K = 1000, threshold = 0.01
// K = 50, threshold = 0.06
#define K 1000
#define mtrain 542
#define mCrossValidation 542
#define threshold 0.01

string ClusterMean[K];
vector<vector<string>> WordsInCluster(K);
int currentHist[K];
int trainHist[mtrain][K];

int LevDist(string s1, string s2) // lev. distance between two strings
{
	int m = s1.length();
	int n = s2.length();
	int L[m + 1][n + 1];
	for (int i = 0; i <= m; i++)
	{
		for (int j = 0; j <= n; j++)
		{
			if (i == 0)
				L[i][j] = j;
			else if (j == 0)
				L[i][j] = i;
			else if (s1[i - 1] == s2[j - 1])
				L[i][j] = L[i - 1][j - 1];
			else
				L[i][j] = 1 + min(L[i][j - 1], min(L[i - 1][j], L[i - 1][j - 1]));
		}
	}
	return L[m][n];
}

long long int HashWord(string s)
{
	int len = s.length();
	long long int val = 0;
	for (int i = 0; i < len; i++)
	{
		val = val + int(s[i]) * pow(13, i);
		val = val % (1000000007);
	}
	return val;
}

int findClosest(string s)
{
	int min = INT_MAX, index = 0;
	for (int i = 0; i < K; i++)
	{
		int d = LevDist(s, ClusterMean[i]);
		if (d < min)
		{
			min = d;
			index = i;
		}
	}
	return index;
}

void UpdateMean(int index)
{
	int sz = WordsInCluster[index].size();
	long long int hash[sz];
	double average = 0.0;
	for (int i = 0; i < sz; i++)
	{
		hash[i] = HashWord(WordsInCluster[index][i]);
		average = average + hash[i];
	}
	average = average / sz;
	int min = INT_MAX;
	for (int i = 0; i < sz; i++)
	{
		int d = abs(hash[i] - average);
		if (d < min)
		{
			min = d;
			ClusterMean[index] = WordsInCluster[index][i];
		}
	}
}

void printRepWords()
{
	cout << "\nRepresentative words.\n";
	for (int i = 0; i < K; i++)
		cout << i << ".\t|\t" << ClusterMean[i] << "\n";
	cout << "\n";
}

void FindClusterMean(vector<string> words)
{
	int total_words = words.size();
	for (int i = 0; i < K; i++) // randomly select K words
	{
		int rnd = rand() % total_words;
		ClusterMean[i] = words[rnd];
		WordsInCluster[i].push_back(words[rnd]);
	}
	for (int i = 0; i < total_words; i++)
	{
		int closest = findClosest(words[i]);
		if (words[i] != ClusterMean[closest])
		{
			WordsInCluster[closest].push_back(words[i]);
			UpdateMean(closest);
		}
	}
	return;
}

void computeHistogram(vector<string> sentence)
{
	int sen_size = sentence.size();
	for (int i = 0; i < K; i++)
		currentHist[i] = 0;
	for (int i = 0; i < sen_size; i++)
	{
		int index = findClosest(sentence[i]);
		currentHist[index]++;
	}
	return;
}

string makecompat(string ch)
{
	string s = "";
	char c;
	int len = ch.length();
	for (int i = 0; i < len; i++)
	{
		if (ch[i] >= 'A' && ch[i] <= 'Z')
			c = (ch[i] - 'A' + 'a');
		else if (ch[i] >= 'a' && ch[i] <= 'z')
			c = ch[i];
		else if (ch[i] >= '0' && ch[i] <= '9')
			c = '*';
		else
		{
			c = '\0';
			break;
		}
		s = s + c;
	}
	if (s == "the" || s == "are")
		return "";
	return s;
}

double HistAngleDifference(int a[], int b[])
{
	double cos = 0, anorm = 0.0, bnorm = 0.0, ip = 0.0;
	for (int i = 0; i < K; i++)
	{
		anorm = anorm + (a[i] * a[i]);
		bnorm = bnorm + (b[i] * b[i]);
		ip = ip + (a[i] * b[i]);
	}
	anorm = sqrt(anorm);
	bnorm = sqrt(bnorm);
	if (bnorm == 0)
		cos = -2;
	if (anorm == 0)
		cos = -2;
	if (anorm == 0 && bnorm == 0)
		cos = 1;
	else
		cos = ip / (anorm * bnorm); // cos => [-1,1] as large the cos is closer Hists are.
	return cos;
}

int findNearestHist(int arr[])
{
	double max1 = -1.1, max2 = -1.1;
	int ind = 0, ind2 = 0;
	for (int i = 0; i < mtrain; i++)
	{
		double cos = HistAngleDifference(arr, trainHist[i]);
		if (cos > max1)
		{
			max1 = cos;
			ind = i;
			if (i <= 40)
			{
				max2 = max1;
				ind2 = ind;
			}
		}
	}
	if (max1 == -1.1)
		ind = 40 + (rand() % 501); // guess ham
	if ((max1 - max2) <= threshold && max1 != max2)
		return ind2;
	return ind;
}

int main()
{
	ifstream input;
	vector<string> words;

	input.open("train.txt");
	cout << "\nReading from train file...\n";
	string ch;
	int cntspamtrain = 0, cnthamtrain = 0;
	while (input >> ch)
	{
		if (cntspamtrain >= 41 && cnthamtrain >= 501)
			break;
		string s = makecompat(ch);
		if (s.length() >= 3 && s.length() <= 15)
			words.push_back(s);
		int l = ch.length();
		if (l >= 3 && ch[l - 1] == 'm' && ch[l - 2] == 'a' && ch[l - 3] == 'p')
			cntspamtrain++;
		if (l >= 3 && ch[l - 1] == 'm' && ch[l - 2] == 'a' && ch[l - 3] == 'h')
			cnthamtrain++;
	}
	input.close();

	cout << "\nSpam train examples---" << cntspamtrain << ", Ham train examples---" << cnthamtrain << "\n";

	cout << "\nComputing K_means...\n";
	FindClusterMean(words);
	cout << "\nK_means computation succesfully done...\n";
	//printRepWords();

	cout << "\nComputing histograms...\n";
	input.open("train.txt");
	int cnt = 0;
	vector<string> sentence;
	while (input >> ch)
	{
		if (cnt >= mtrain)
			break;
		string s = makecompat(ch);
		if (s.length() >= 3 && s.length() <= 15)
			sentence.push_back(s);
		int l = ch.length();
		if (l >= 3 && ch[l - 1] == 'm' && ch[l - 2] == 'a' && (ch[l - 3] == 'p' || ch[l - 3] == 'h'))
		{
			computeHistogram(sentence);
			for (int j = 0; j < K; j++)
				trainHist[cnt][j] = currentHist[j];
			cnt++;
			sentence.clear();
		}
	}
	input.close();

	cout << "\nTraining histograms succesfully completed...\n";

	cout << "\nTESTING on cross validation examples (41 spam and 501 ham examples)...\n";
	float Precision = 0.0, Recall = 0.0, F1score = 0.0, Accuracy = 0.0; // spam => (y=1); ham => (y=0)
	int TP = 0, TN = 0, FP = 0, FN = 0;									// T=True, F=False, P=Positive, N=Negative
	ifstream cv;
	cv.open("crossValidation.txt");
	int mcv = 0;
	sentence.clear();
	while (cv >> ch)
	{
		if (mcv >= mCrossValidation)
			break;
		string s = makecompat(ch);
		if (s.length() >= 3 && s.length() <= 15)
			sentence.push_back(s);
		int l = ch.length();
		if (l >= 3 && ch[l - 1] == 'm' && ch[l - 2] == 'a' && (ch[l - 3] == 'h' || ch[l - 3] == 'p'))
		{
			computeHistogram(sentence);
			int indHist = findNearestHist(currentHist);
			if (ch[l - 3] == 'p') // actual spam
			{
				if (indHist <= 40) // predicted spam
					TP++;
				else // predicted ham
					FN++;
			}
			else if (ch[l - 3] == 'h') // actual ham
			{
				if (indHist <= 40) // predicted spam
					FP++;
				else // predicted ham
					TN++;
			}
			mcv++;
			sentence.clear();
		}
	}

	cout << "\nResults\n\n";
	printf("TP         |    %d\n", TP);
	printf("FP         |    %d\n", FP);
	printf("FN         |    %d\n", FN);
	printf("TN         |    %d\n", TN);
	if (TP != 0)
	{
		Precision = TP / float(TP + FP);
		Recall = TP / float(TP + FN);
		F1score = (2 * Precision * Recall) / (Precision + Recall);
	}
	Accuracy = (TP + TN) / float(mCrossValidation);
	printf("Precision  |    %0.6f\n", Precision);
	printf("Recall     |    %0.6f\n", Recall);
	printf("F1score    |    %0.6f\n", F1score);
	printf("Accuracy   |    %0.6f\n", Accuracy);

	return 0;
}