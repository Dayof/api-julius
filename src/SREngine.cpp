/*
 * SREngine.cpp
 *
 *  Created on: Nov 11, 2009
 *      Author: 08080002701
 */

#include <SREngine.h>
#include <RecoResult.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

namespace LapsAPI {

typedef struct pWait{
	pthread_mutex_t *mutexPaused;
	pthread_cond_t *condPaused;
} PWait;

void wait(Recog *recog, void *dummy){
	PWait *p = (PWait*) dummy;
	pthread_mutex_lock(p->mutexPaused);
	pthread_cond_wait(p->condPaused, p->mutexPaused);
	pthread_mutex_unlock(p->mutexPaused);
}

void *recognizeStream(void *dummy) {
	Recog* recog = (Recog*) dummy;
	/**********************/
	/* Recognization Loop */
	/**********************/
	/* enter main loop to recognize the input stream */
	/* finish after whole input has been processed and input reaches end */
	int ret;
	ret = j_recognize_stream(recog);
	if (ret == -1)
		return NULL; /* error */

	/*******/
	/* End */
	/*******/

	/* calling j_close_stream(recog) at any time will terminate
	 recognition and exit j_recognize_stream() */
	//SREngine::OnRecognition->Invoke(gcnew LapsAPI::RecoResult("FECHOU", 9));
	j_close_stream(recog);
	//FILE* f = jlog_get_fp();
	//fprintf(f, "sai");
	//jlog_flush();
	return 0;
}

void startOnRecognize(Recog *recog, void *dummy) {
	void (*actionRecognize)(RecoResult *recoResult);
	actionRecognize = (void(*)(RecoResult*)) dummy;
	Jconf *jconf = recog->jconf;
	WORD_INFO *winfo;
	WORD_ID *seq;
	int seqnum;
	int n;
	Sentence *s;
	RecogProcess *r;
	string uterrance;
	/* all recognition results are stored at each recognition process
	 instance */
	for (r = recog->process_list; r; r = r->next) {

		/* skip the process if the process is not alive */
		if (!r->live)
			continue;

		/* result are in r->result.  See recog.h for details */

		/* check result status */
		if (r->result.status < 0) { /* no results obtained */
			/* outout message according to the status code */
			switch (r->result.status) {
			case J_RESULT_STATUS_REJECT_POWER:
				printf("<input rejected by power>\n");
				break;
			case J_RESULT_STATUS_TERMINATE:
				printf("<input teminated by request>\n");
				break;
			case J_RESULT_STATUS_ONLY_SILENCE:
				printf("<input rejected by decoder (silence input result)>\n");
				break;
			case J_RESULT_STATUS_REJECT_GMM:
				printf("<input rejected by GMM>\n");
				break;
			case J_RESULT_STATUS_REJECT_SHORT:
				printf("<input rejected by short input>\n");
				break;
			case J_RESULT_STATUS_FAIL:
				printf("<search failed>\n");
				break;
			}
			/* continue to next process instance */
			continue;
		}

		/* output results for all the obtained sentences */
		winfo = r->lm->winfo;

		for (n = 0; n < r->result.sentnum; n++) { /* for all sentences */
			s = &(r->result.sent[n]);
			seq = s->word;
			seqnum = s->word_num;
			uterrance = "";
			/* output word sequence like Julius */
			for (int i = 0; i < seqnum; i++) {
				uterrance += " ";
				uterrance += winfo->woutput[seq[i]];
			}
			float confidence = 0;
			for (int i = 1; i < seqnum - 1; i++) {
				confidence += s->confidence[i];
			}
			confidence /= ((float) seqnum - 2);
			//printf(" %s", winfo->woutput[seq[i]]);
			//RecognizedAction::Invoke(gcnew RecogResult(uterrance, confidence));
			//Recognizer::OnRecognized(gcnew RecogResult(uterrance, confidence));
			//Recognizer::RecognizedActionDele::Invoke(gcnew RecogResult(uterrance, confidence));
			if (actionRecognize != NULL)
				actionRecognize(new RecoResult(uterrance, confidence));
			//SREngine::OnRecognition->Invoke(gcnew RecoResult(uterrance, confidence));
		}
	}
}

SREngine::SREngine() {
	recog = j_recog_new();
	jconf = j_jconf_new();
	//actionRecognize = NULL;
}

SREngine::SREngine(char *jconfPath) {

	FILE *log;
	log = fopen("JuliusLog", "w");
	//log = fopen_writefile("JuliusLog");
	/*
	 fputc('c', log);
	 fflush(log);
	 fclose(log);
	 */

	jlog_set_output(log);
	//jlog_set_output(fopen("Juliuslog", "w"));

	//Load configurations file
	//jconf = j_config_load_file_new( Util().string2Char(jconfPath) );
	jconf = j_config_load_file_new(jconfPath);
	if (jconf == NULL) {
		printf("Error cannot load jconf\n");
		//throw gcnew System::ApplicationException("Error cannot load jconf");
		return;
	}
	//jconf = j_config_load_file_new("G:\\AM\\julius.jconf");
	//Force use mic (jconf file do not will care)
	jconf->input.type = INPUT_WAVEFORM;
	jconf->input.speech_input = SP_MIC;
	jconf->input.device = SP_INPUT_DEFAULT;
	jconf->decodeopt.realtime_flag = TRUE;

	recog = j_create_instance_from_jconf(jconf);
	if (recog == NULL) {
		printf("Error cannot create an instance\n");
		//throw gcnew System::ApplicationException("Error cannot create an instance");
		return;
		//cout << "Error cannot create an instance" << endl;
	}
	/*
	 * Pass the function that will be executed when something is reconized
	 * and is passed one pointer to this class to know how recognizer is
	 * executing for more information look startOnRecnized function
	 */
	//callback_add(recog, CALLBACK_RESULT, startOnRecognize, NULL);

	pthread_mutex_init(&mutexPaused, NULL);
	pthread_cond_init(&condPaused, NULL);
	paused = false;

	PWait *p = (PWait*) malloc(sizeof(PWait));
	p->mutexPaused = &mutexPaused;
	p->condPaused = &condPaused;
	callback_add(recog, CALLBACK_PAUSE_FUNCTION, wait, (void*) p);
}

SREngine::~SREngine() {
	//Free memory of recog including jconf
	j_recog_free(recog);
}

void SREngine::startRecognition() {
	if (paused){
		j_request_resume(recog);
		pthread_mutex_lock(&mutexPaused);
		pthread_cond_signal(&condPaused);
		pthread_mutex_unlock(&mutexPaused);
		return;
	}

	//if already exists any process running donot start again
	if (recog->process_online == TRUE) {
		return;
	}

	//j_request_resume(recog);
	if (j_adin_init(recog) == FALSE) { /* error */
		return;
	}

	/* output system information to log */
	j_recog_info(recog);

	/***********************************/
	/* Open input stream and recognize */
	/***********************************/

	/* raw speech input (microphone etc.) */

	switch (j_open_stream(recog, NULL)) {
	case 0: /* succeeded */
		break;
	case -1: /* error */
		fprintf(stderr, "error in input stream\n");
		return;
	case -2: /* end of recognition process */
		fprintf(stderr, "failed to begin input stream\n");
		return;
	}

	int rcThreadRecog;
	rcThreadRecog = pthread_create(&threadRecog, NULL, recognizeStream,
			(void*) recog);
	if (rcThreadRecog){
		fprintf(stderr, "error while create thread for recognize stream loop");
		return;
	}

	/*
	 HANDLE recogLoopThread;
	 DWORD dwRecogLoopThread;
	 recogLoopThread = CreateThread(NULL, 0, recognizeStream, recog, 0, &dwRecogLoopThread);
	 if (recogLoopThread == NULL){
	 fprintf(stderr, "error while create thread for recognize stream loop");
	 return;
	 }
	 */
}

void SREngine::stopRecognition() {
	if (recog->process_online == TRUE) {
		j_request_terminate(recog);
		paused = true;
	}
}

void SREngine::setOnRecognizeAction(void(*func)(RecoResult *recogRestult)) {
	this->actionRecognize = func;
	callback_add(recog, CALLBACK_RESULT, startOnRecognize,
			(void*) this->actionRecognize);
}
}
