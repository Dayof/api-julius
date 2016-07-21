#include <SREngine.h>
#include <RecoResult.h>
#include <iostream>

using namespace LapsAPI;
using namespace std;

void reconheceu(RecoResult *result){
	cout << result->getUterrance() << " Confiança " << result->getConfidence() << endl;
}

int main(){
	SREngine* en;
	en = new SREngine("/home/day/Desktop/myjulius/clara.jconf");
	en->setOnRecognizeAction(&reconheceu);
	en->startRecognition();
	printf("Reconhecendo\n");
	sleep(10);
	printf("Parado\n");
	en->stopRecognition();
	sleep(10);
	en->startRecognition();
	printf("Reconhecendo\n");
	sleep(100);
}
