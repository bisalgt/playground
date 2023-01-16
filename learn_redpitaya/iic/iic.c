
/* This code is sued for starting continous ranging on SRF02 sensor
 * 
 * (c) Red Pitaya  http://www.redpitaya.com
 *
 * This part of code is written in C programming language.
 * Please visit http://en.wikipedia.org/wiki/C_(programming_language)
 * for more details on the language used herein.
 */
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <math.h>

// daemon includes
#include <pthread.h>
#include <syslog.h>

#include "rp.h"
#include "_kiss_fft_guts.h"
#include "kiss_fft.h"
#include "kiss_fftr.h"


//	global defines
#define I2C_SLAVE_FORCE 		   0x0706
#define I2C_SLAVE    			   0x0703    /* Change slave address            */
#define I2C_FUNCS    			   0x0705    /* Get the adapter functionality */
#define I2C_RDWR    			   0x0707    /* Combined R/W transfer (one stop only)*/

#define	V_SONIC_WAVE				(float)(343.2)											// [m/s] Schallgeschwindigkeit in Luft
#define V_SONIC_WAVE_CM_PER_SEC		(uint32_t)(V_SONIC_WAVE * 100)							// [cm/s] --> 343,2 m/s * 100 = 34.200 cm/s
#define MAX_CHAR					(uint8_t)20

#define ADC_MAX_SAMPLE_FREQUENCY	125000000												// [Hz] --> 125 MHz
#define ADC_SAMPLE_DECIMATION		64														// [-]
#define ADC_SAMPLE_FREQUENCY		( ADC_MAX_SAMPLE_FREQUENCY / ADC_SAMPLE_DECIMATION )
#define ADC_SAMPLE_TIME				8														// [ns]
#define ADC_SAMPLE_TIME_NS			(uint32_t)( ADC_SAMPLE_DECIMATION * ADC_SAMPLE_TIME )	// [ns] --> 8*64=512 ns / sample
#define	ADC_START_DELAY_OFFSET_US	270													// [µs]
#define FFT_MIN_FREQ				35000
#define FFT_MAX_FREQ				45000													// [Hz]
#define FFT_WINDOW_STD				4096													// FFT window width in samples [-]
#define FREQ_CORRECTION				0														// [Hz]
#define MIN_DISTANCE				30														// [cm]
#define MIN_DISTANCE_MAX			500														// [cm]

#define F1_FILTER_WIDTH				10
#define F2_FILTER_WIDTH				10
#define F6_FILTER_WIDTH				10
#define F9_FILTER_WIDTH				10
#define P_STEP						(sizeof(uint16_t))

#define THREAD_PAUSE_MS				(100)													//     50 ms
#define THREAD_PAUSE_US				(THREAD_PAUSE_MS*1000)									// 50.000 µs = 1 ms (per cycle)
#define MIN_PROG_PAUSE_MS			( THREAD_PAUSE_MS )										//    100 ms
//Neuronal Net related defines
//*
#define HIDDEN_LAYERS				3														// number of hidden Layers 
#define HIDDEN_LAYER_SIZE			40*HIDDEN_LAYERS										//größe der hidden Layer (muss möglicherweise angepasst werden)
#define GRADIENT_LIMIT				30														//TODO: Testen was am besten passt
//*/


typedef enum
{
	ERR_NO				=  0,
	ERR_LOGIC			= -1,
	ERR_FILE_CREATION	= -2,
	ERR_FILE_OPEN		= -3,
	ERR_DATA			= -4,
	ERR_COM				= -5,
	ERR_I2C				= -6,
	ERROR_FILE			= -7,
	ERR_TCP				= -8,
	ERR_FATAL			= -127
}error_t;

typedef enum
{
	NET_ERR_NO = 0,
	NET_ERR_INIT_L=-101,
	NET_ERR_INIT_E = -102,
	NET_ERR_INIT_W = -103,
	NET_ERR_FT = -10,
	NET_ERR_FT_E = -11,
	NET_ERR_HL = -12,
	NET_ERR_HL_E = -13,
	NET_ERR_W = -14,
	NET_ERR_W_E = -15,
	NET_ERR_CL = -16,
	NET_ERR_CL_E = -17,
	NET_ERR_EP = -18,
	NET_ERR_GR	=-20
}net_error_t;

/* Typedefs for global structs: adc, fft and udp */
typedef struct
{
	uint16_t	w_width;
	uint16_t	w_start;
	uint16_t	start_delay_us;
	 int16_t	start_delay_us_offset;
	uint16_t	sumup_delay_us;
	uint16_t	meas_delay_us;
	uint16_t	min_dist_cm;
	double		max_dist_cm;
	int16_t		delta_ix;
	uint16_t	ampl_thrshld;
	uint16_t	ampl_max;
	uint16_t	ampl_max_ix;
	uint32_t	buffer_size;
	double		distance_factor;
	 int16_t	*data;
}adc_t;

typedef struct
{
	uint16_t	w_width;
	uint16_t	freq_min;			//35000
	uint16_t	freq_max;			//45000
	uint16_t	freq_center;
	uint16_t	freq_min_index;
	uint16_t	freq_max_index;
	uint16_t	freq_center_index;
	double		freq_factor;
	uint16_t	*data;
}fft_t;


typedef struct
{
	int			socket;
	socklen_t 	length;
	char		command;
	struct		sockaddr_in serveraddr, clientaddr;
	int32_t		parameter[2];
}udp_t;


typedef struct
{
	int			fd;
	char		buf[10];
}iic_t;



typedef struct
{
	uint16_t	idx;
	uint16_t	idx_max;
	double		latest_val;
	double		sum;
	double		filt_val;
	double		*buffer;
}filter_t;

//weights,error,layer,...
typedef struct 
{
	uint16_t		class_cnt;
	uint16_t		feature_cnt;
	uint16_t		class_predict;
	char			*class_names[10];
	int				layer_size[2+HIDDEN_LAYERS];
	int				*epoch;
	int				*trained;
	float			**layer;
	float			**error;
	float			**weight;
}net_param_t;




// global variables definition
static adc_t				adc_s;
static fft_t				fft_s;
static udp_t				udp_s={-1,-1};
static iic_t				iic_s;
static filter_t				filter_distance;
static uint32_t				adc_pause_ms;
// net related global vars
static net_param_t			nets_param_s; 

// variable s for I/O
static const uint32_t		thread_pause_us = THREAD_PAUSE_US;
static double				distance;
static bool					b_run_server	= true;
static bool					b_send_fft		= false;
static bool					b_send_adc		= false;
static bool					b_meas_dist		= false;
pthread_mutex_t				run_meas_mutex;
pthread_cond_t				run_meas_cond;
rp_dpin_t					led_0 = RP_LED0;
rp_dpin_t					led_1 = RP_LED1;
rp_dpin_t					led_2 = RP_LED2;
rp_pinDirection_t			led_dir = RP_OUT;

int _epoch = 0;
int _class = 0;
int _initialized = 0;
int _pred = 0;
uint8_t get_fft = 0;
uint8_t clear = 0;
uint16_t* fft_Buff;
float learn=0.1;
float decay=0.9;

FILE* file = NULL;
char* file_name = "/root/redpitaya/Examples/Communication/C/iic/net.txt";
uint8_t _saving = 0, _loading = 0, _FL = 0;
/*
*
*	LD_LIBRARY_PATH=/opt/redpitaya/lib ./iic
*
*/
void	ServerThread( void *threadid );
void	MeasureThread( void *threadid );
void	TimeThread( void *threadid );
//void	send_via_udp(char* data_in);

void send_via_udp(char* data_in);
int main(int argc, char **argv)
{
	pthread_t t_ServerThread, t_MeasureThread, t_TimeThread;
	pthread_attr_t attr;
	void *th_status;
	
	// kill the parent-process
	// and fork the current execution to force a new PID and
	// start as a daemon
	daemon(0, 0);
	
	
	// start the udp server task in background!
	// set attribute for detaching status for each thread!
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
	
	
	// init all mutexes which are needed
	pthread_mutex_init( &run_meas_mutex, NULL );
	pthread_cond_init( &run_meas_cond, NULL );
	
	
	// create and start the threads
	pthread_create( &t_ServerThread,	&attr, (void *)&ServerThread,	NULL );
	pthread_create( &t_MeasureThread,	&attr, (void *)&MeasureThread,	NULL );
	pthread_create( &t_TimeThread,		&attr, (void *)&TimeThread,		NULL );
	
	
	// free the attribute again
	pthread_attr_destroy( &attr );
	
	
	// wait for both threads to terminate
	// go into blocked mode
	pthread_join( t_ServerThread,	&th_status );
	pthread_join( t_MeasureThread,	&th_status );
	pthread_join( t_TimeThread,		&th_status );
	
	
	// end main as last thread!
	pthread_exit( NULL );
	return( EXIT_SUCCESS );
}



void init_adc(void)
{
	rp_Init();
	rp_DpinSetDirection( RP_DIO0_P, RP_IN);							// DIO0 as input
	rp_DpinSetState( RP_DIO0_P, RP_LOW);							// DIO0 set to low
	//rp_AcqSetDecimation( RP_DEC_64 );								// Decimation 64 --> 64 * 8 ns = 512 ns / sample
	rp_AcqSetSamplingRate( RP_SMP_1_953M );							// Sample rate 1.953Msps; Buffer time length 8.388ms; Decimation 64
	rp_AcqSetTriggerSrc( RP_TRIG_SRC_DISABLED );					// Trigger disabled
	rp_AcqSetArmKeep( false );
	rp_AcqSetTriggerDelay( 0 );
	rp_AcqStart();
	rp_AcqStop();
}


void init_net_param_struct(net_param_t *nets_param_s)
{
	nets_param_s->class_cnt = 0;
	nets_param_s->class_predict = 0;
	nets_param_s->feature_cnt = 0;
	for(int i=0;i<HIDDEN_LAYERS+2;i++)
	{
		nets_param_s->layer_size[i]=0;
	}
}
// ************************************************************ //
// ******************** Filter functions ********************** //
// ************************************************************ //

/*
*	@brief:		This function is used to initialize a filter of given length
*	@details:	The function needs a struct and a count of single
				measurements to allocate memory as much as needed.
				It sums up all values over 'count_in' measurements and
				provides a floating mean-value.
	@params:	filter_t *f: a pointer to an uninitialized filter struct
	@params:	uint16_t count_in: a count for single 
*/
void init_filter( filter_t *f, uint16_t count_in )
{
	f->idx_max 	= count_in;
	f->idx		= 0;
	f->sum		= 0;
	f->buffer	= malloc( count_in * sizeof(double) );
}


// universal running average filter for x values
void run_mean_filter( filter_t *f, double *val_in )
{
	f->sum -= f->buffer[ f->idx ] - *val_in;
	f->buffer[ (f->idx)++ ] = *val_in;
	f->idx %= f->idx_max;
	f->filt_val = (double)(f->sum / f->idx_max);
}



void set_min_distance_and_adc_start_delay( adc_t *a, uint32_t min_dist_cm_in, int32_t adc_start_delay_us_offset_in )
{
	double factor;
	a->min_dist_cm				= ( min_dist_cm_in > MIN_DISTANCE_MAX )?( MIN_DISTANCE_MAX ):( ( min_dist_cm_in < MIN_DISTANCE )?(MIN_DISTANCE):(min_dist_cm_in) );
	a->start_delay_us_offset	= ( ( adc_start_delay_us_offset_in >= -2000 ) && ( adc_start_delay_us_offset_in <= 2000 ) )?(adc_start_delay_us_offset_in):(ADC_START_DELAY_OFFSET_US);
	
	a->max_dist_cm				= ((a->buffer_size * ADC_SAMPLE_TIME_NS) * V_SONIC_WAVE) / 2e7 + a->min_dist_cm;
	a->start_delay_us			=  a->start_delay_us_offset + (uint32_t)(( a->min_dist_cm * 2 * 1e6 ) / V_SONIC_WAVE_CM_PER_SEC );
	a->meas_delay_us			= (uint32_t)(( ADC_SAMPLE_TIME_NS * a->buffer_size ) / 1e3);
	a->sumup_delay_us			=  a->start_delay_us + adc_s.meas_delay_us;
	factor						= ((double)a->min_dist_cm / (double)MIN_DISTANCE);
	a->ampl_thrshld				= (uint16_t)(1000.0 / factor) + 150;
}



void set_min_max_fft_frequency( adc_t *a, fft_t *f, uint16_t freq_min_in, uint16_t freq_max_in, int32_t w_width_in )
{
	if( w_width_in > 0 ){
		w_width_in -= w_width_in % 2;
		f->w_width	= ( w_width_in >= 1024 && w_width_in <= 8192 )?( (uint16_t)w_width_in):(FFT_WINDOW_STD);
		a->w_width	= f->w_width;
	}
	
	f->freq_factor	 	= (double)( ADC_SAMPLE_FREQUENCY / (double)(f->w_width * 2.0 ) );
	f->freq_min 		= ( freq_min_in < FFT_MIN_FREQ )?( FFT_MIN_FREQ ):( freq_min_in );
	f->freq_max 		= ( freq_max_in > FFT_MAX_FREQ )?( FFT_MAX_FREQ ):( freq_max_in );
	
	f->freq_min_index = (uint32_t)( (double)(f->freq_min / f->freq_factor) );
	f->freq_max_index = (uint32_t)( (double)(f->freq_max / f->freq_factor) );
}


/*
* @brief	This function is called, if ADC data has to be generated
*
* @details	The function calculates time differences, which are needed for 
*			starting and stopping the measurement depending on needed distance. 
* 			
*
*
*/
int start_adc( adc_t *a )
{
	// I²C programming - start ultrasonic
	if ((write(iic_s.fd, iic_s.buf, 2)) != 2) {
		exit(ERR_I2C);
	}

	// wait for sonic to pass by 
	// the minimum distance
	usleep( a->start_delay_us );

	// ADC is started
	rp_AcqStart();

	// wait until buffer is full
	usleep( a->meas_delay_us );

	// ADC is stopped
	rp_AcqStop();

	// get data out of ADC buffer.
	rp_AcqGetLatestDataRaw( RP_CH_1, &a->buffer_size, a->data );
	
	return(0);
}




/*
* @brief	This function measures the distance to an object
*
* @details	The function searches the maximum amplitude and
*			its position. 
*
*
*
*/
void measure_distance( adc_t *a, double *distance_out )
{	
	// calculate the distance of the measured object
	// and write back to distance_out pointer
	if( a->ampl_max_ix > 0 ){
		// any kind of object is recognized
		*distance_out = ((double)( a->distance_factor * a->ampl_max_ix) +  (double)( a->min_dist_cm ) / 100) ;
	}
	else{
		*distance_out = -1.0;
	}
}



int set_window_adc_data( adc_t *a, double *val_out )
{
	int ret=2;
	uint32_t	*length		= &a->buffer_size;
	int16_t		val;
	uint16_t	*max_amp = &a->ampl_max;
	uint16_t	*max_amp_ix = &a->ampl_max_ix;
	
	*max_amp		= 0;
	*max_amp_ix		= 0;
	
	for( uint16_t ix=0; ix < *length; ix++){
		val = abs(a->data[ix]);
		if( val > a->ampl_thrshld ){
			if( val > *max_amp ){
				*max_amp_ix		= ix;
				*max_amp		= val;
			}
		}
	}
	
	if( *max_amp > 0 ){
		*val_out = (double)((double)*max_amp_ix / (double)*length) * 143.95 - (143.95/2);
		if( *val_out < -5.0 ){
			ret = -1;
		}
		else if( *val_out > 5.0 ){
			ret = 1;
		}
		else{
			a->w_start = *max_amp_ix - (a->w_width / 2);
			ret = 0;
		}
	}

	
	return(ret);
}






/*
* @brief		This function executes the FFT for given ADC values
* 
* @details		The funntion needs a signed integer 16 bit pointer to an array of the length <length>
*				and a pointer to an array where the result(s) will be stored.
*
* @length		length: the amount of data, which will be analized
* @data_in		*data_in: values between -2^12...2^12 ( array of adc values)
* @data_out		*data_out: pointer to array of the length (length/2+1)
*
*/
int calc_fft(adc_t *a, fft_t *f )
{
	// create an output buffer for FFT
	// create a config variable for FFT
	uint32_t length = f->w_width;
	int16_t *data_in = &(a->data[a->w_start]);
	send_via_udp("calc");
	kiss_fft_cpx* fft_out = (kiss_fft_cpx*)calloc((length), sizeof(kiss_fft_cpx)); // nicht nachvollziehbarer crash 	

	kiss_fftr_cfg	fft_cfg;
	double re, im;
	double max_amplitude = 0.0;
	uint16_t offset = 1;
	
	
	// allocate temporary space for calculation
	fft_cfg = kiss_fftr_alloc(length,0,NULL,NULL);
	if (fft_cfg == NULL)
	{
		send_via_udp("problem");
	}
	
	
	// execute the FFT
	kiss_fftr( fft_cfg, (kiss_fft_scalar *) data_in, fft_out );
	
	
	// build the sum of imaginary and real part of the FFT
	// and search the max_amplitude amplitude
	for( uint16_t ix = offset; ix < (length/2); ix++ ){
		re = fft_out[ix].r, im = fft_out[ix].i;
		fft_out[ix-offset].r = (uint32_t)(sqrtf( re*re + im*im ));
		if( fft_out[ix-offset].r > max_amplitude ){
			max_amplitude = (double)fft_out[ix-offset].r;
			f->freq_center_index = (ix-offset);
		}
	}
	
	
	
	f->freq_center = f->freq_center_index * f->freq_factor;
	max_amplitude /= 1000;
	
	// normalize data
	for( uint16_t ix = offset; ix < (length/2); ix++ ){
		f->data[ix-offset] = (uint16_t)((double)fft_out[ix].r / max_amplitude);
	}
	
	free( fft_out );
	free( fft_cfg );
	
	return(ERR_NO);
}

void send_via_udp(char* data_in)
{
	if (udp_s.socket > 0) {
		sendto(udp_s.socket, data_in, strlen(data_in), MSG_DONTWAIT, (struct sockaddr*)&udp_s.clientaddr, udp_s.length);
	}
}

//UNSER BEREICH FUNKTIONEN

int net_param_init(net_param_t* nets_param_s, uint16_t classCnt, uint16_t featureCnt)
{
	
	char msg[65];
	send_via_udp("initialize Net");
	nets_param_s->class_cnt = classCnt;
	nets_param_s->feature_cnt = featureCnt;
	sprintf(msg, "class: %i,feature: %i", classCnt, featureCnt);
	send_via_udp(msg);

	for (int i = 0; i < classCnt; i++)
	{
		nets_param_s->class_names[i] = (char*)malloc(10 * sizeof(char));
	}
	
	nets_param_s->layer_size[0] = featureCnt;
	nets_param_s->epoch = (int*)malloc(classCnt *sizeof(int));
	if (nets_param_s->epoch == NULL) {
		return NET_ERR_EP;
	}
	else send_via_udp("--epoch");
	nets_param_s->trained = (int*)malloc(classCnt * sizeof(int));
	if (nets_param_s->trained == NULL) {
		return NET_ERR_EP;
	}
	else send_via_udp("--trained");

	for (int i = 0; i < classCnt; i++)
	{
		nets_param_s->epoch[i] = 1;
		nets_param_s->trained[i] = 1;
	}
	sprintf(msg, "Layer_size[0]:%i", nets_param_s->layer_size[0]);
	send_via_udp(msg);
	for (int i = 1; i <= HIDDEN_LAYERS; i++)
	{
		nets_param_s->layer_size[i] = HIDDEN_LAYER_SIZE/i;
		sprintf(msg, "Layer_size[%i]:%i", i, nets_param_s->layer_size[i]);
		send_via_udp(msg);
	}

	nets_param_s->layer_size[HIDDEN_LAYERS + 1] = classCnt;		// Bsp.{23,500,500,3}
	sprintf(msg, "Layer_size[%i]:%i", HIDDEN_LAYERS + 1, nets_param_s->layer_size[HIDDEN_LAYERS + 1]);
	send_via_udp(msg);
	if (nets_param_s->layer == NULL) {
		send_via_udp("Layers clear");
	}
	nets_param_s->layer = (float**)malloc((2 + HIDDEN_LAYERS) * sizeof(float*));
	if (nets_param_s->layer == NULL)return NET_ERR_INIT_L;
	else
	{
		send_via_udp("--layer_alloc");
	}
	nets_param_s->error = (float**)malloc((2 + HIDDEN_LAYERS) * sizeof(float*));
	if (nets_param_s->error== NULL)return NET_ERR_INIT_E;
	else
	{
		send_via_udp("--error_alloc");
	}
	nets_param_s->weight = (float**)malloc((2 + HIDDEN_LAYERS) * sizeof(float*));
	if (nets_param_s->weight == NULL)return NET_ERR_INIT_W;
	else
	{
		send_via_udp("--weight_alloc");
	}
	nets_param_s->layer[0] = (float*)malloc((nets_param_s->layer_size[0] + 1) * sizeof(float));
	if (nets_param_s->layer[0] == NULL)return NET_ERR_FT;
	else
	{
		send_via_udp("--layer[0]");
	}
	nets_param_s->error[0] = (float*)malloc((nets_param_s->layer_size[0]) * sizeof(float));
	if (nets_param_s->error[0] == NULL)return NET_ERR_FT_E;
	else {
		send_via_udp("--error[0]");
	}
	//möglicherweise wieder entfernen
	nets_param_s->weight[0] = (float*)malloc(sizeof(float));
	if (nets_param_s->weight[0] == NULL)return NET_ERR_W;
	else
	{
		send_via_udp("--weight[0]");
	}
	//
	for (int i = 1; i < 2 + HIDDEN_LAYERS; i++)
	{
		nets_param_s->layer[i] = (float*)malloc((nets_param_s->layer_size[i] + 1) * sizeof(float));
		if ((nets_param_s->layer[i] == NULL) && (i < HIDDEN_LAYERS + 1))return NET_ERR_HL;
		else if ((nets_param_s->layer[i] == NULL) && (i == HIDDEN_LAYERS + 1))return NET_ERR_CL;
		else {
			sprintf(msg, "--layer[%i]", i);
			send_via_udp(msg);
		}
		nets_param_s->error[i] = (float*)malloc((nets_param_s->layer_size[i]) * sizeof(float));
		if ((nets_param_s->error[i] == NULL) && (i < HIDDEN_LAYERS + 1))return NET_ERR_HL_E;
		else if ((nets_param_s->error[i] == NULL) && (i == HIDDEN_LAYERS + 1))return NET_ERR_CL_E;
		else {
			sprintf(msg, "--error[%i]", i);
			send_via_udp(msg);
		}
		nets_param_s->weight[i] = (float*)malloc(((nets_param_s->layer_size[i]) * (nets_param_s->layer_size[i - 1]) + 1) * sizeof(float));
		if ((nets_param_s->weight[i] == NULL) && (i <= HIDDEN_LAYERS + 1))return NET_ERR_W;
		else {
			sprintf(msg, "--weight[%i]", i);
			send_via_udp(msg);
		}

	}

	float scale; 
	scale = 0.0f;
	static int layerSize = 0;
	
	for (int i = 0; i < 2 + HIDDEN_LAYERS; i++)
	{

		layerSize = nets_param_s->layer_size[i];
		nets_param_s->layer[i][layerSize] = 1.0f;
	}
	
	for (int j = 1; j < 2+ HIDDEN_LAYERS; j++) 
	{
		//HE INITIALIZATION
		scale = sqrtf((2.0f) / nets_param_s->layer_size[j - 1]);
		
		if (j!=1+HIDDEN_LAYERS)scale = scale * 1.41; // RELU	
		for (int i = 0; i < (nets_param_s->layer_size[j]) * (nets_param_s->layer_size[j - 1] + 1); i++)
		{
			nets_param_s->weight[j][i] = scale * ((float)rand() / RAND_MAX - 0.5);
		}
		
		for (int i = 0; i < (nets_param_s->layer_size[j]); i++) // BIASES
		{
			nets_param_s->weight[j][(nets_param_s->layer_size[j - 1] + 1) * (i + 1) - 1] = 0.0;
			//sprintf(msg, "bias init: %2.2f", nets_param_s->weight[j][(nets_param_s->layer_size[j - 1] + 1) * (i + 1) - 1]);
			//send_via_udp(msg);
		}

		
	}
	_initialized = 1;
	return NET_ERR_NO;

}
/*
#define a
#define b
//global
struct {
...

...
} name_t
name_t var;

main()

init(name_t* f_var, int s1, int s2)
{
	...
	f_var->error = (float**)malloc(a*sizeof(float));
	...
	for(){
		f_var->error[i]=(flaot*)malloc(b*sizeof(float));
		...
		}

}
(free() ?)
func(name_t* f_var)
{
	for()
	{
		free(f_var->error[i]);
		...
	}
	free(f_var->error);
}
keine debug möglichkeiten

*/

void map_names(net_param_t *nets_param_s,int class_nbr , char* name)
{

	for (int i = 0; i < 10; i++) {
		nets_param_s->class_names[class_nbr][i] = name[i];
		
	}

}

// save net structure
int save_weights(net_param_t* nets_param_s, int layer)
{
	int location = 0;
	if ((file = fopen(file_name, "a")) < 0) {
		send_via_udp("error");
		return ERR_FILE_OPEN;
	}
	for (int i = 0; i < nets_param_s->layer_size[layer]; i++)
	{
		for (int j = 0; j < nets_param_s->layer_size[layer - 1] + 1; j++)
		{
			location = i * (nets_param_s->layer_size[layer - 1]) + j;
			fprintf(file, "%2.6f\n", nets_param_s->weight[layer][location]);
		}
	}

	fclose(file);
	return layer + 1;
}
/*
* @brief	This function is used to save data into RAM of RedPitaya
*
* @details	The function creates a new file with an index n and
*			stores the input *data_in_adc and *data_in_fft into a file.
*			Data is seperated by tabs to have a microsoft compatible format.
*
* @return	0: success; -1: not defined; -2: file creation error; -3: file open error
*
*/

int save_data(net_param_t* nets_param_s)
{
	int i;
	
	send_via_udp("save_data");

	if ((file = fopen(file_name, "w")) < 0) {
		send_via_udp("error");
		return ERR_FILE_OPEN;
	}

	//write line into file
	fprintf(file, "feature_cnt:%d\n", nets_param_s->feature_cnt);
	fprintf(file, "class_cnt:%d\n", nets_param_s->class_cnt);
	fprintf(file, "class_names:\n");
	for (i = 0; i < nets_param_s->class_cnt; i++)
	{
		fprintf(file, "%s,\n", nets_param_s->class_names[i]);	
	}
	fprintf(file, "epochs:\n");
	for (i = 0; i < nets_param_s->class_cnt; i++)
	{
		fprintf(file, "%i\n", nets_param_s->epoch[i]);
	}
	fprintf(file, "trained:\n");
	for (i = 0; i < nets_param_s->class_cnt; i++)
	{
		fprintf(file, "%i\n", nets_param_s->trained[i]);
	}
	//for (i = 0; i < 2 + HIDDEN_LAYERS; i++)fprintf(file,"%d\n",nets_param_s->layer_size[i]);
	
	fprintf(file, "weights:\n");

	fclose(file);
	
	
	//fclose(file);

	return 1;
}

int loading_data(net_param_t* nets_param_s)
{
	char msg[40];
	int i = 0;
	int j = 0; 
	int location = 0;
	int line_cnt = 0;
	send_via_udp("loading initialized");

	if ((file = fopen(file_name, "r")) < 0)
	{
		send_via_udp("error");
		return ERR_FILE_OPEN;
	}
	int lLength = 20;
	char line[lLength];
	char* c;


	fgets(line, lLength, file);				// get feature count 
	line_cnt++;
	if ((c = strstr(line, ":")) != NULL) {
		nets_param_s->feature_cnt = atoi(++c);
	}
	sprintf(msg, "%s", line);
	send_via_udp(msg);
	fgets(line, lLength, file);				//get class count
	line_cnt++;
	if ((c = strstr(line, ":")) != NULL) {
		nets_param_s->class_cnt = atoi(++c);
	}
	sprintf(msg, "%s", line);
	send_via_udp(msg);
	sprintf(msg, "f:%i, c:%i", nets_param_s->feature_cnt, nets_param_s->class_cnt);
	send_via_udp(msg);
	usleep(100);
	net_param_init(nets_param_s, (uint16_t)nets_param_s->class_cnt, (uint16_t)nets_param_s->feature_cnt);
	_FL = 1;
	fgets(line, lLength, file);				//== "class_name:" unimportant--> skip line
	line_cnt++;
	for (i = 0; i < nets_param_s->class_cnt; i++)
	{
		j = 0;
		fgets(line, lLength, file);			//get class name 
		line_cnt++;
		c = strtok(line, ",");
		while (c != NULL) {
			
			if (j == 0)
			{
				sprintf(msg, "%s", c);
				send_via_udp(msg);
				map_names(nets_param_s, i, c);
			}
			c = strtok(NULL, ",");
			j++;
		}
		
		
	}

	fgets(line, lLength, file);				//== "epochs:" unimportant--> skip line	
	line_cnt++;
	for (i = 0; i < nets_param_s->class_cnt; i++)
	{
		fgets(line, lLength, file);
		line_cnt++;
		nets_param_s->epoch[i] = atoi(line);
	}
	
	fgets(line, lLength, file);				//== "train:" unimportant--> skip line	
	line_cnt++;
	for (i = 0; i < nets_param_s->class_cnt; i++)
	{
		fgets(line, lLength, file);
		line_cnt++;
		nets_param_s->trained[i] = atoi(line);
		sprintf(msg, "%i", nets_param_s->trained[i]);
		send_via_udp(msg);
	}

	fgets(line, lLength, file);				//== "weights:" unimportant--> skip line
	line_cnt++;
	for (int layer = 1; layer < 1 + HIDDEN_LAYERS; layer++)
	{
		for (i = 0; i < nets_param_s->layer_size[layer]; i++)
		{
			for (j = 0; j < nets_param_s->layer_size[layer - 1] + 1; j++)
			{
				fgets(line, lLength, file);
				line_cnt++;
				location = i * (nets_param_s->layer_size[layer - 1]) + j;
				nets_param_s->weight[layer][location] = atof(line);
			}		
		}
	}
	sprintf(msg, "line_cnt: %i", line_cnt);
	send_via_udp(msg);
	
	fclose(file);
	sprintf(msg, "%i", _FL);
	send_via_udp(msg);
	send_via_udp("doneso");
	return 0;
}

int forwardProp(net_param_t *nets_param_s)
{
	char msg[40];
	float sum=0.0;
	float esum = 0.0;
	float max=0.0;
	int i, j, k;
	int imax = 0;
	// Input/Feature Layer
	for (i = 0; i < (nets_param_s->layer_size[0]); i++)
	{
		nets_param_s->layer[0][i] = (float)fft_Buff[i]/1000;
		
	}
	//Hidden Layer -ReLU activation
	for (k = 1; k < 1+HIDDEN_LAYERS; k++)			//access Hidden Layers
		for (i = 0; i < nets_param_s->layer_size[k]; i++) {			//access values of Hidden Layer
			sum = 0.0;

			for (j = 0; j < nets_param_s->layer_size[k - 1] + 1; j++)
			{
				sum += nets_param_s->layer[k - 1][j] * nets_param_s->weight[k][i * (nets_param_s->layer_size[k - 1] + 1) + j];
			}	
			if (sum >= 0.0)
			{
				nets_param_s->layer[k][i] = sum;
			}
			else
			{
				nets_param_s->layer[k][i] = 0.0;
			}
			
		
		}
	// OUTPUT LAYER - SOFTMAX ACTIVATION
	esum = 0.0;
	for (i = 0; i < nets_param_s->layer_size[1+HIDDEN_LAYERS]; i++) {
		sum = 0.0;
		for (j = 0; j < nets_param_s->layer_size[HIDDEN_LAYERS] + 1; j++)
		{
			sum +=nets_param_s->layer[HIDDEN_LAYERS][j] * nets_param_s->weight[1+HIDDEN_LAYERS][i * (nets_param_s->layer_size[HIDDEN_LAYERS] + 1) + j];
		}
		if (sum >(float)GRADIENT_LIMIT) return NET_ERR_GR; //GRADIENT EXPLODED
		nets_param_s->layer[1+HIDDEN_LAYERS][i] = expf(sum);
		esum += nets_param_s->layer[1 + HIDDEN_LAYERS][i];
	}
	// SOFTMAX FUNCTION
	max = nets_param_s->layer[1+HIDDEN_LAYERS][0]; imax = 0;
	for (i = 0; i < nets_param_s->layer_size[1+HIDDEN_LAYERS]; i++) {
		
		if (nets_param_s->layer[1+HIDDEN_LAYERS][i] > max) {
			max = nets_param_s->layer[1 + HIDDEN_LAYERS][i];
			imax = i;
		}
		nets_param_s->layer[1+HIDDEN_LAYERS][i] = nets_param_s->layer[1+HIDDEN_LAYERS][i] / esum;
		sprintf(msg, "Out: %2.3f at: %d", nets_param_s->layer[1 + HIDDEN_LAYERS][i], i);
		send_via_udp(msg);
	}
	return imax;
}

int backwardProp(net_param_t *nets_param_s, int is_class)
{
	char msg[40];
	int  i, j, k, p, r=0; 
	//int total_epochs=0;
	float der = 1.0;
	float pow_scaler=1.0;

	
	p = forwardProp(nets_param_s);
	if (p == NET_ERR_GR) {
		return NET_ERR_GR;
	}
	if (p == is_class) // test if prediction and class are identical
	{
		r = 1;
	}
	// OUTPUT LAYER - CALCULATE ERRORS
	for (i = 0; i < nets_param_s->layer_size[1+HIDDEN_LAYERS]; i++) {
		pow_scaler = (nets_param_s->epoch[i])/(nets_param_s->trained[i]);
		nets_param_s->error[1 + HIDDEN_LAYERS][i] = learn * (0 - nets_param_s->layer[1 + HIDDEN_LAYERS][i]) * powf(decay, 1+pow_scaler);
		
		if (i == is_class) {
			nets_param_s->error[1 + HIDDEN_LAYERS][i] = learn * (1 - nets_param_s->layer[1 + HIDDEN_LAYERS][i]) * powf(decay, 1 + pow_scaler);
			sprintf(msg, "e: %2.3f lr: %2.3f epoch: %d", nets_param_s->error[1 + HIDDEN_LAYERS][i], nets_param_s->error[1 + HIDDEN_LAYERS][i], nets_param_s->epoch[i]);
			send_via_udp(msg);
			nets_param_s->epoch[i] += 1;
			if (nets_param_s->layer[1 + HIDDEN_LAYERS][i] == 0) return NET_ERR_GR; // GRADIENT VANISHED
		} 	
	}
	// HIDDEN LAYERS - CALCULATE ERRORS
	
	for (k = HIDDEN_LAYERS; k > 0 ; k--) {
		for (i = 0; i < nets_param_s->layer_size[k]; i++) {
			nets_param_s->error[k][i] = 0;
			if (nets_param_s->layer[k][i] > 0) // ReLU DERIVATIVE
			{
				for (j = 0; j < nets_param_s->layer_size[k + 1]; j++) {
					nets_param_s->error[k][i] += nets_param_s->error[k + 1][j] * nets_param_s->weight[k + 1][j * (nets_param_s->layer_size[k] + 1) + i] * der;
				}
			}
			
					
		}
	}
	// UPDATE WEIGHTS - GRADIENT DESCENT
	for (k = 1; k < 2+HIDDEN_LAYERS; k++)
	{
		for (i = 0; i < nets_param_s->layer_size[k]; i++)
		{
			for (j = 0; j < nets_param_s->layer_size[k - 1] + 1; j++)
			{
				nets_param_s->weight[k][i * (nets_param_s->layer_size[k - 1] + 1) + j] += nets_param_s->error[k][i] * nets_param_s->layer[k - 1][j];
			}
			
		}
	}
				
	return r;
}


void send_binary( uint16_t *data_in, uint16_t count )
{
	uint16_t Sent;
	while( count > 0 ){
		Sent = sendto( udp_s.socket, data_in, sizeof(uint16_t) * count, MSG_DONTWAIT,  (struct sockaddr *) &udp_s.clientaddr, udp_s.length)/2;
		count -= Sent, data_in += Sent;
	}
}



void send_binary_int16( int16_t *data_in, uint16_t count )
{
	uint16_t Sent;
	while( count > 0 ){
		Sent = sendto( udp_s.socket, data_in, sizeof(uint16_t) * count, MSG_DONTWAIT,  (struct sockaddr *) &udp_s.clientaddr, udp_s.length)/2;
		count -= Sent, data_in += Sent;
	}
}




void parse_udp_message( char *msg_rx )
{
	char *rx_param;
	char msg_tx[40];
	int classe=0, feature=0,retval=0;
	net_param_t *ptr;
	fft_t *f;
	f = &fft_s;
	ptr = &nets_param_s;
	//int save_succ =0;
	
	
	
	if( msg_rx[0] == '-' ){
		
		udp_s.command = msg_rx[1];
		if( (rx_param = strstr( msg_rx, " ")) != NULL ){
			udp_s.parameter[0] = atoi( rx_param++ );
			if( (rx_param = strstr( rx_param, " ")) != NULL ){
				udp_s.parameter[1] = atoi( rx_param );
			}
		}
		
		switch( udp_s.command )
		{
			case 'a':
				b_send_adc = ( udp_s.parameter[0] == 1 );
				if( b_send_adc ){
					adc_pause_ms = (uint32_t)(udp_s.parameter[1]);
					adc_pause_ms -= adc_pause_ms % THREAD_PAUSE_MS;
				}
			break;
			case 'v':
				sprintf(msg_tx,"SW version: V0.12c");
				send_via_udp(msg_tx);
			break;
			case 's':
				b_meas_dist = ( udp_s.parameter[0] == 1 );
			break;
			case 'f':
				b_send_fft = ( udp_s.parameter[0] == 1 );
			break;
			case 'i':
				init_adc();
			break;
			case 'w':
				// set new window width
				set_min_max_fft_frequency( &adc_s, &fft_s, fft_s.freq_min, fft_s.freq_max, (int32_t)abs(udp_s.parameter[0]) );
			break;
			case 'q':
				// set the flag for stopping threads and the whole process
				b_run_server = false;
			break;

			case 'n':
				//TODO: if f->fre.. ==NULL -->Error
				
				feature = f->freq_max_index - f->freq_min_index;
				retval = net_param_init(&nets_param_s, (uint16_t)udp_s.parameter[0], (uint16_t)feature);
				classe =ptr->class_cnt;
				sprintf(msg_tx, "retval: %d", retval);
				send_via_udp(msg_tx);
				sprintf(msg_tx, "class_cnt: %d,features: %d", classe, feature);
				send_via_udp(msg_tx);
				
				

			break;
			case 'c':
				if ((rx_param = strstr(msg_rx, " ")) != NULL) {
					udp_s.parameter[0] = atoi(rx_param++);
					if ((rx_param = strstr(rx_param, " ")) != NULL) {
						map_names(&nets_param_s, udp_s.parameter[0], ++rx_param);
						sprintf(msg_tx, "class mapped! %s", rx_param);
						send_via_udp(msg_tx);
					}
				}
			break;
			case 'g':
				rx_param = ptr->class_names[udp_s.parameter[0]];
				sprintf(msg_tx, "class: %s", rx_param);
				send_via_udp(msg_tx);
				//udp_s.parameter[0]//klasse die abgefragt wird
				
			break;
			case 't':
				_class = (int)udp_s.parameter[0];
				_epoch = (int)udp_s.parameter[1];
				_pred = 0;
				nets_param_s.trained[_class] += 1;

			break;
			case 'b':
				if (!_loading)_saving = 1;
				else send_via_udp("loading in progress please wait");

			break;
			case 'l': //for you lost sensor!!
				if (!_saving) _loading = 1;
				else send_via_udp("saving in progress please wait");
				if (_loading)
				{
					_saving = 0;
					
					if (_initialized)
					{
						for (int i = 0; i < ptr->class_cnt; i++)
						{
							free(ptr->class_names[i]);
							send_via_udp("freetest_class");
						}
						for (int i = 0; i < 2 + HIDDEN_LAYERS; i++)
						{
							free(&(ptr->error[i]));
							ptr->layer[i] = NULL;
							ptr->weight[i] = NULL;
							if (ptr->error[i] == NULL)send_via_udp("free e");
						}
						free(ptr->error);
						ptr->error = NULL;
						ptr->layer = NULL;
						ptr->weight = NULL;
						ptr->epoch = NULL;
						ptr->trained = NULL;
						init_net_param_struct(&nets_param_s);
					}

					loading_data(&nets_param_s);

					
				}
				break;
			default:
				sprintf( msg_tx, "unknown command %c", udp_s.command );
				send_via_udp(msg_tx);
			break;
		} // end switch udp_s.command
	}	
}



// Thread No. 1 for UDP!
/*
*	@brief		Thread waits for UDP message(s) in blocked mode
*				and parses the received message(s) and value(s).
*	@name		ServerThread
*/
void ServerThread( void *threadid )
{
	char rx_buffer[25];
	memset( &udp_s.serveraddr,	0, sizeof(udp_s.serveraddr) );
	memset( &udp_s.clientaddr,	0, sizeof(udp_s.clientaddr) ); 
	
	udp_s.serveraddr.sin_family = AF_INET;
	udp_s.serveraddr.sin_port = htons( 6000 );
	udp_s.serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	udp_s.length = sizeof(udp_s.clientaddr);
	
	// Create a socket with UDP protocol
	udp_s.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	bind( udp_s.socket, (struct sockaddr *)&udp_s.serveraddr, sizeof(udp_s.serveraddr));
	
	while( b_run_server ){
		memset( (void *)rx_buffer, (int)0, (size_t)(strlen( rx_buffer )) );
		// Go into blocked mode to free CPU; wait for UDP message
		if( recvfrom( udp_s.socket, rx_buffer, sizeof(rx_buffer), 0, (struct sockaddr *)&udp_s.clientaddr, &udp_s.length ) < 0 ){
			break;
		}
		
		parse_udp_message( rx_buffer );
		
	} // end while()
	
	close( udp_s.socket );
	pthread_exit(NULL);
}






// Thread No. 2 for measuring!
/*
*	@brief		Thread waits for signal from ServerThread in blocked mode
*				and processes the udp_s.command(s). 
*	@name		MeasureThread
*/
void MeasureThread( void *threadid )
{
	// variables for IIC
	int						address		= 0x70;					// Address of the SRF08 shifted right 1 bit
	char					*fileName	= "/dev/i2c-0";			// Name of the port we will be using	
	char* rx_msg;
	net_param_t* ptr;
	ptr = &nets_param_s;

	// variables for FFT and measurement
	int						retval;								// return value for all adc/fft functions to proof distance of possible object
	uint32_t				counter_ms		= 0;
	uint32_t				min_distance_cm	= 0;
	int cnt = 0;
	int prediction = 0;
	int layer = 0;
	
	
	// variables for upd communication
	char msg_tx[60];
	
	rp_DpinSetDirection( led_0, led_dir );
	rp_DpinSetDirection( led_1, led_dir );
	
	
	// initialize a filter for the distannce measurement
	init_filter( &filter_distance, 3 );
	init_net_param_struct(&nets_param_s);
	
	// set the length of the buffer for ADC
	adc_s.buffer_size			= (uint32_t)ADC_BUFFER_SIZE;		// 16.384
	
	
	// variable to calculate a distance with a given maximum adc index
	adc_s.distance_factor = (double)( ( (double)(V_SONIC_WAVE) / 2.0) * (double)( ADC_SAMPLE_TIME_NS ) ) / 1e9 ;	// [ m / sample]
	
	
	// Open I²C port for reading and writing
	if ((iic_s.fd = open(fileName, O_RDWR)) < 0) {						
		exit(1);
	}
	
	
	// Set the port options and set the address of the device we wish to speak to
	if (ioctl(iic_s.fd, I2C_SLAVE_FORCE, address) < 0) {					
		exit(1);
	}
	
	
	// init and calib the adc before starting!
	init_adc();
	
	
	// initialize minimum distance, adc_delay offset and max/min fft frequency
	set_min_distance_and_adc_start_delay( &adc_s, MIN_DISTANCE, ADC_START_DELAY_OFFSET_US );
	set_min_max_fft_frequency( &adc_s, &fft_s, FFT_MIN_FREQ, FFT_MAX_FREQ, (int32_t)FFT_WINDOW_STD );
	
	
	while( b_run_server )
	{
		send_via_udp("server still running");
		double offset=0.0;
		fft_s.data = (uint16_t *)calloc( adc_s.buffer_size/2 + 1, sizeof(uint16_t));
		adc_s.data = ( int16_t *)calloc( adc_s.buffer_size		, sizeof(int16_t));
		send_via_udp("fft and adc allocated");
		iic_s.buf[0] = 0;
		iic_s.buf[1] = 81;
		
		do{
			send_via_udp("do something");
			counter_ms += THREAD_PAUSE_MS;
			// wait for signal from ServerThread
			// thread will be set into blocked mode
			pthread_cond_wait(&run_meas_cond, &run_meas_mutex);
			
			start_adc( &adc_s );
			send_via_udp("adc start");
			
			// retval can be -1,-0,1 and 2
			// -1: object is present, but too near from center of buffer
			//  0: object is in optimal position inside ADC data
			//  1: object present, but too far from center of buffer
			//  2: there is no threshold amplitude reached --> no object!
			retval = set_window_adc_data( &adc_s, &offset );
			
			if( retval != 0 ){
				send_via_udp("Distance stuff");
				rp_DpinSetState( led_1, RP_LOW );
				if( retval > 1 ){
					rp_DpinSetState( led_0, RP_LOW );
					min_distance_cm = abs( min_distance_cm + 150 * retval ) % MIN_DISTANCE_MAX;
				}
				else{
					min_distance_cm = abs( min_distance_cm + offset ) % MIN_DISTANCE_MAX;
				}
				set_min_distance_and_adc_start_delay( &adc_s,MIN_DISTANCE + min_distance_cm , ADC_START_DELAY_OFFSET_US );
			}			
			
		}while(retval != 0);
		send_via_udp("LED and stuff");
		
		rp_DpinSetState( led_0, RP_HIGH );
		rp_DpinSetState( led_1, RP_HIGH );
		
		
		
		// start the FFT
		send_via_udp("Random crash incoming in");
		send_via_udp("3...");
		send_via_udp("2...");
		send_via_udp("1...");
		calc_fft( &adc_s, &fft_s);
		if (_FL>0)
		{
			send_via_udp("free");
			_initialized = 1;
			_loading = 0;
		}
		if (_initialized&!(_saving||_loading))
		{
			fft_Buff = (uint16_t*)calloc(adc_s.buffer_size / 2 + 1, sizeof(uint16_t));
			
			for (int i = 0; i < (fft_s.freq_max_index - fft_s.freq_min_index) + 1; i++)
			{
				fft_Buff[i] = fft_s.data[fft_s.freq_min_index + i];
			}
			if (_pred)
			{
				prediction=forwardProp(&nets_param_s);
				rx_msg = ptr->class_names[prediction];
				sprintf(msg_tx, "Prediction: %s", rx_msg);
				send_via_udp(msg_tx);
				usleep(10 * thread_pause_us);

			}
			else if (!(_pred)&&(cnt<_epoch))
			{
				backwardProp(&nets_param_s,_class);
				
				sprintf(msg_tx, "epoche: %d", cnt);
				send_via_udp(msg_tx);
				usleep(10 * thread_pause_us);
				cnt++;
			}
			else if(cnt==_epoch)
			{
				_pred=1;
				cnt = 0;
			}
			else {
				sprintf(msg_tx, "HILFE");
				send_via_udp(msg_tx);
			}
				
			

		}
		if (_saving)
		{
			_loading = 0;
			if (layer == 0)
			{
				layer = save_data(&nets_param_s);
			}
			else if (layer < HIDDEN_LAYERS + 1)
			{
				layer = save_weights(&nets_param_s, layer);
				
			}
			else
			{
				_saving = 0;
				layer = 0;
				send_via_udp("save successful");
			}
		}
		

		
		if(b_send_fft){
			send_via_udp(" # ");
			send_binary( (uint16_t *)&fft_s.data[fft_s.freq_min_index], (fft_s.freq_max_index - fft_s.freq_min_index ) + 1);
		}
		else if(b_send_adc){
			if( counter_ms % adc_pause_ms == 0 ){
				send_via_udp(" # ");
				send_binary_int16( (int16_t *)adc_s.data, ADC_BUFFER_SIZE );
			}
		}
		else if(b_meas_dist){
			measure_distance( &adc_s, &distance );
			run_mean_filter( &filter_distance, &distance );
			if( counter_ms % 300 == 0 ){
				sprintf( msg_tx, "%.2f m", filter_distance.filt_val );
				send_via_udp( msg_tx );
			}
			
		}
		
		
		free( fft_s.data );
		free( adc_s.data );
		if (_initialized & !(_saving || _loading))
		{
			free(fft_Buff);
		}
		
	} // end while iic_s.run_server
	

	
	rp_Release();
	
	pthread_exit(NULL);
}




void TimeThread( void *threadid )
{
	while(b_run_server)
	{
		usleep( thread_pause_us );
		pthread_cond_signal(&run_meas_cond);
	}
}



