#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_SIZE 64
#define PI	M_PI
#define TWOPI	(2.0*PI)

float Ans[MAX_SIZE+1];//Vetor para armazenar os valores temporários das operações com os vetores

/* Constantes para a transformada rápida de fourier */
int NFFT_t;
float *X, *X_ABS, *f, Fs, *X_ANG, N_HARM;

//Integração pela regra dos trapézios
float myTrapz(float *X, float *Y, unsigned int size){
	//myTrapz Função para cálculo da integral numérica de uma função discreta
	//   Efetua uma aproximação usando o método de soma de trapézios.
	int i=0;
	float Int=0, H, B, b;
	for(i=0; i< size-1; i++){
		H = X[i+1]-X[i];
		B = Y[i];
		b = Y[i+1];
		Int = Int + (B+b)*H/2;
	}
	return Int;
}

//Multiplicação entre vetores
float* vectorMult(float *X, float *Y, unsigned int size){
	int i=0;
	for(i=0;i<size;i++){
		Ans[i] = X[i]*Y[i];
	}
	return Ans;
}

//Eleva ao quadrado cada termo do vetor
float* vectorPow(float *X, unsigned int size){
	int i=0;
	for(i=0;i<size;i++){
		Ans[i] = X[i]*X[i];
	}
	return Ans;
}

//Calculo do fator de potência
void powerFactor(float *t, float *V, float *I, unsigned int size, float *fp, float *S, float *P, float *Vrms, float *Irms){
	/*powerFactor Função para calculo do fator de potência
%   Essa função retorna o fator de potência (fp), potência ativa(P) e
%   potêcia aparente(S) com base no tempo(t), tensão(V) e corrente(I).
%   Todos os valores de entrada devem ser vetores vindos da amostragem.
%   Esses valores devem também serem correspondentes a um período de
%   amostra, para estimativa correta dos resultados.*/
	float fp_t=0, S_t=0, P_t=0, Vrms_t=0, Irms_t=0;
	//type_double_vec Ans[5];
	float T = t[size-1]-t[0];
	P_t = myTrapz(t, vectorMult(V, I, size), size)/T;
	Vrms_t = sqrt(myTrapz(t, vectorPow(V, size), size)/T);
	Irms_t = sqrt(myTrapz(t, vectorPow(I, size), size)/T);
	S_t = Vrms_t*Irms_t;
	fp_t = P_t/S_t;

	*fp=fp_t;
	*S=S_t;
	*P=P_t;
	*Vrms=Vrms_t;
	*Irms=Irms_t;
}

//Cálculo da transformada rápida de fourier
void four1(float data[], int nn, int isign)
{
    int n, mmax, m, j, istep, i;
    float wtemp, wr, wpr, wpi, wi, theta;
    float tempr, tempi;

    n = nn << 1;
    j = 1;
    for (i = 1; i < n; i += 2) {
	if (j > i) {
	    tempr = data[j];     data[j] = data[i];     data[i] = tempr;
	    tempr = data[j+1]; data[j+1] = data[i+1]; data[i+1] = tempr;
	}
	m = n >> 1;
	while (m >= 2 && j > m) {
	    j -= m;
	    m >>= 1;
	}
	j += m;
    }
    mmax = 2;
    while (n > mmax) {
	istep = 2*mmax;
	theta = TWOPI/(isign*mmax);
	wtemp = sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi = sin(theta);
	wr = 1.0;
	wi = 0.0;
	for (m = 1; m < mmax; m += 2) {
	    for (i = m; i <= n; i += istep) {
		j =i + mmax;
		tempr = wr*data[j]   - wi*data[j+1];
		tempi = wr*data[j+1] + wi*data[j];
		data[j]   = data[i]   - tempr;
		data[j+1] = data[i+1] - tempi;
		data[i] += tempr;
		data[i+1] += tempi;
	    }
	    wr = (wtemp = wr)*wpr - wi*wpi + wr;
	    wi = wi*wpr + wtemp*wpi + wi;
	}
	mmax = istep;
    }
}

//Cálculo da taxa de distorção harmônica
float THD(float *I, unsigned int NFFT, unsigned int size){
    int Nx, i;
	float maximum, sum;
	int location;
	Nx=size;

    /* Storing x(n) in a complex array to make it work with four1.
	This is needed even though x(n) is purely real in this case. */
	for(i=0; i<Nx; i++){
		X[2*i+1] = I[i];//******************************************Input signal
		X[2*i+2] = 0.0;
	}
	/* pad the remainder of the array with zeros (0 + 0 j) */
	for(i=Nx; i<NFFT; i++){
		X[2*i+1] = 0.0;
		X[2*i+2] = 0.0;
	}

	/*printf("\nInput complex sequence (padded to next highest power of 2):\n");
	for(i=0; i<NFFT; i++){
		printf("x[%d] = (%.2f + j %.2f)\n", i, X[2*i+1], X[2*i+2]);
	}*/

	/* calculate FFT */
	four1(X, NFFT, 1);

    /* Compute the X signal ABS*/
	for(i=0; i<NFFT; i++){
		X_ANG[2*i+1]=sqrt(pow(X[2*i+1],2)+pow(X[2*i+2],2));
		X_ABS[i]=sqrt(pow(X[2*i+1],2)+pow(X[2*i+2],2));
		//X_ABS[i]=pow(X[2*i+1],2)+pow(X[2*i+2],2);
		X_ANG[2*i+2]=tan((float)X[2*i+2]/(float)X[2*i+1]);
	}
	for(i=0; i<NFFT; i++){
		f[i]=i*((float)Fs/(float)NFFT);
	}
	//maximum=maximum/414.812;
	for (i = 0; i < NFFT/2; i++){
        X_ABS[i]=X_ABS[i]/(NFFT);
    }
	maximum = 0.0;
	location=0;
    for (i = 0; i < NFFT/2; i++){
        if (X_ABS[i] > maximum){
           maximum  = X_ABS[i];
           location = i;
        }
    }
    sum=0;
	for(i=location+1; i<=N_HARM; i+=1){
        sum+=pow(X_ABS[i],2);
	}

    /**********************************************/
	FILE *file;
	file = fopen("saidas.txt", "w");
	//printf("\nX FFT:\n");
	/*for(i=0; i<NFFT; i++){
		printf("X[%d] = (%.2f + j %.2f)\n", i, X[2*i+1], X[2*i+2]);
	}*/

    printf("\nABS FFT:\n");
    fprintf(file, "X_ABS=[");
	for(i=0; i<NFFT; i++){
		//printf("X[%d] = (%.2f /_ %.2f)\n", i, X_ANG[2*i+1], ((float)180/(float)M_PI)*X_ABS[2*i+2]);
		fprintf(file, "%f ", X_ABS[i]);
	}
	fprintf(file, "]\nf=[");
	for(i=0; i<NFFT; i++){
		//printf("f[%d] = %.2f\n", i, f[i]);
		fprintf(file, "%f ", f[i]);
	}
	fprintf(file, "]\nX_ANG=[");
	for(i=0; i<NFFT; i++){
	    fprintf(file,"%.2f ",X_ANG[i]);
	}
	fprintf(file, "]");
	fclose(file);
    /**********************************************/
    printf("\nAmplitude fundamental: %f\n", maximum);
    return (sqrt((float)sum)*10)/((float)maximum);
}

//Detector de cruzamento por zero, retorna ao primeiro cruzamento ascendente
int zeroCross(float *X, unsigned int size){
    /*%zeroCross Detecção de cruzamentos por zero
    %   Detecta cruzamentos e os divide em bordas de descidas e subidas do
    %   vetor X*/
    //QList<int> t_down;
    //QLIst<int> t_up;
    int i=0, j=0, k=0;
    for (i=0; i<size; i++){
        if (X[i]>=0 && X[i+1]<=0){
            return i;
            j++;
        }
        /*if (X[i]<=0 && X[i+1]>=0){
            return i;
            k++;
        }*/
    }
}

//Gera os sinais discretos de tempo, tensão, corrente e cálculo da frequência de amostragem; com base nas impedâncias dos circuitos RL e RC.
void signalGen(float *t, float *V, float *I, unsigned int size, float *Fs, float Vrms, float freq, unsigned int type, float R, float L, float C){
    unsigned int i;
    //t_inter=0.00026;//t_inter = T/MAX_SIZE = (1/freq)/MAX_SIZE
    float t_inter = (1/freq)/MAX_SIZE;
    *Fs=1/t_inter;
    float Z, Ip, theta;

    for(i=0; i<MAX_SIZE; i++){
        //t[i] = ((float)i/(float)MAX_SIZE)*t_inter;
        t[i] =i*t_inter;
    }
    //RL
    if(type==0){
        Z = sqrt(pow(R,2)+pow(2*PI*freq*L,2));
        Ip = (Vrms/Z)*sqrt(2);
        theta= atan((-2*PI*freq*L)/(R));
        for (i=0; i<MAX_SIZE; i++){
          V[i] = Vrms*sqrt(2)*sin(2*M_PI*freq*t[i]);
          I[i] = Ip*sin((2*M_PI*((float)i/(float)MAX_SIZE))+(theta));//((float)M_PI/(float)180)
        }
    }
    //RC
    else if(type==1){
        Z = sqrt(pow(R,2)+pow(1/(2*PI*freq*C),2));
        Ip = (Vrms/Z)*sqrt(2);
        theta= atan((1)/(2*PI*freq*C*R));
        for (i=0; i<MAX_SIZE; i++){
          V[i] = Vrms*sqrt(2)*sin(2*M_PI*freq*t[i]);
          I[i] = Ip*sqrt(2)*sin((2*M_PI*((float)i/(float)MAX_SIZE))+(theta));//*((float)M_PI/(float)180)
        }
    }
    //RMMO
    else{
        Z=R;
        theta=0;
        Ip=(Vrms*sqrt(2))/R;
        for (i=0; i<MAX_SIZE; i++){
          V[i] = Vrms*sqrt(2)*sin(2*M_PI*freq*t[i]);
          //Corrente não linear: retificador monofásico meia onda
          if(i<MAX_SIZE/2)
            I[i] = Ip*sin(2*M_PI*freq*t[i]);
          else I[i]=0;
        }
    }

    printf("->criacao do sinal...\n");
    printf("->Fs: %f, t_inter: %f, Z: %f, \n->theta: %f, Ip: %f\n", *Fs, t_inter, Z, theta, Ip);
}

//Função principal
int main(){
    //COntador e ponteiro para arquivos
    int i;
    FILE *file;
    printf("Teste do calculo do fator de potencia e demais rotinas\nAutor: Angelo Polotto\n");
    //Declaração das variáveis
    float t[MAX_SIZE], V[MAX_SIZE], I[MAX_SIZE];
    float fp, S, P, Vrms, Irms, freq=60;//Definição da frequência do sinal de entrada
    float V_rms_source=127, R=43.5, L=0.300, C=0.000030;//Tensão eficaz, valor de R, L e C da carga
    /*
    type=0 - circuito RL
    type=1 - circuito RC
    type=2 - RMMO
    */
    unsigned int type=2;

    //Variável para a THD
    float THD_val;
    //Fs;//=3840; //Fs=1/t_inter
    NFFT_t = (int)pow(2.0, ceil(log((float)MAX_SIZE)/log(2.0))); //Cálculo do tamanho da FFT
    printf("NFFT = %d\n", NFFT_t);
    N_HARM=10; //N_HARM<NFFT/2 //Calcula até a décima harmônica
    /* allocate memory for NFFT complex numbers (note the +1) */
    X = (float *) malloc((2*NFFT_t+1) * sizeof(float));
    X_ABS = (float *) malloc((2*NFFT_t+1) * sizeof(float));
    X_ANG = (float *) malloc((2*NFFT_t+1) * sizeof(float));
    f = (float *) malloc((NFFT_t+1) * sizeof(float));

    signalGen(t, V, I, MAX_SIZE, &Fs, V_rms_source, freq, type, R, L, C);//Gera os sinais discretos

    printf("->Exportando os dados do sinal...\n");
    file = fopen("entradas.txt", "w");
    fprintf(file, "t=[");
    for ( i = 0; i < MAX_SIZE; i++ ){
        fprintf(file, "%f ", *(t + i));
    }
    fprintf(file, "]\nV=[");
    for ( i = 0; i < MAX_SIZE; i++ ){
        fprintf(file, "%.2f ", *(V + i));
    }
    fprintf(file, "]\nI=[");
    for ( i = 0; i < MAX_SIZE; i++ ){
        fprintf(file, "%.2f ", *(I + i));
    }
    fprintf(file, "]");
    fclose(file);

    printf("->Calculo das variaveis: \n");
    powerFactor(t, V, I, MAX_SIZE, &fp, &S, &P, &Vrms, &Irms);
    printf("->fp: %f, S: %f, P: %f, Vrms: %f, Irms: %f \n", fp, S, P, Vrms, Irms);
    //printf("->Exportando os dados calculados...\n");
    //fprintf(file, "\nfp= %f; S= %f; P= %f; Vrms= %f; Irms= %f; \n", fp, S, P, Vrms, Irms);

    //apuração da THD
	THD_val=THD(I, NFFT_t, MAX_SIZE);
	free(X);
    free(X_ABS);
    free(X_ANG);
    free(f);
    printf("\nI_THD: %f %%\n", THD_val);

    /*printf("V:\n");
    for ( i = 0; i < MAX_SIZE; i++ ){
        printf("%f \n", *(V + i));
    }
    printf("\nI:\n");
    for ( i = 0; i < MAX_SIZE; i++ ){
        printf("%f \n", *(I + i));
    }*/
    //Mostras os pontos de curzamento ascendente por zero
    printf("\nCruzamentos por zero em V: %d\n", zeroCross(V,MAX_SIZE));
    printf("\nCruzamentos por zero em I: %d\n", zeroCross(I,MAX_SIZE));
    return 0;
}
