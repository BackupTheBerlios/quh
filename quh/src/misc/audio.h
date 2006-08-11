typedef struct
{
  uint8_t magic[4];       // 'RIFF'
  uint32_t total_length;  // length of file minus the 8 byte riff header

  uint8_t type[4];        // 'WAVE'

  uint8_t fmt[4];         // 'fmt '
  uint32_t header_length; // length of format chunk minus 8 byte header
  uint16_t format;        // identifies WAVE_FORMAT_PCM
  uint16_t channels;
  uint32_t freq;          // samples per second per channel
  uint32_t bytespersecond;       
  uint16_t blockalign;    // basic block size
  uint16_t bitspersample;

  // PCM formats then go straight to the data chunk
  uint8_t data[4];        // 'data'
  uint32_t data_length;   // length of data chunk minus 8 byte header
} st_audio_wav_t;


typedef struct
{
  int flags;
  
  int freq;
  int bits;
  int channels;
  int is_signed;

  // mem
  void *s;
  int s_len;
  // stream
  FILE *fs;

  int start;
  int len;

  // volume
  int left;
  int right;
} st_audio_t;

