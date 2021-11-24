#ifndef wav_included
#define wav_included
//----------------------------------------------------------------------

// TODO: class-ify

#include <sndfile.h>

struct wav_t {
  int     num_samples;
  float*  buffer_left;
  float*  buffer_right;
};

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

//bool init_wav(wav_t* wav) {
//  wav->num_samples = 0;
//  wav->buffer_left = NULL;
//  wav->buffer_right = NULL;
//  return true;
//}
//
////----------
//
//void delete_wav(wav_t* wav) {
//  wav->num_samples = 0;
//  if (wav->buffer_left) {
//    free(wav->buffer_left);
//    wav->buffer_left = NULL;
//  }
//  if (wav->buffer_right) {
//    free(wav->buffer_right);
//    wav->buffer_right = NULL;
//  }
//}
//
////----------
//
//int load_wav(wav_t* wav, const char* filename) {
//  return 0;
//}
//
////----------
//
//int save_wav(wav_t* wav, const char* filename) {
//  return 0;
//}

//------------------------------
//
//------------------------------

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

SNDFILE* open_infile(SF_INFO* info, const char *path) {
  SNDFILE *snd;
  snd = sf_open(path, SFM_READ, info);
  if (!snd) goto err_sf_open;
  return snd;
err_sf_open:
  fprintf(stderr, "couldn't open \"%s\": %s\n", path, sf_strerror(snd));
  return NULL;
}

//----------

// close..
//  if (test_host.infile) sf_close(test_host.infile);



//----------

SNDFILE* open_outfile(const char *path, int sample_rate, int channels) {
  SNDFILE *snd;
  SF_INFO info = {
    .frames     = 0,
    .samplerate = sample_rate,
    .channels   = channels,
    .format     = SF_FORMAT_WAV | SF_FORMAT_FLOAT,
    .sections   = 0,
    .seekable   = 0
  };
  snd = sf_open(path, SFM_WRITE, &info);
  if (!snd) goto err_sf_open;
  sf_command(snd, SFC_SET_ADD_PEAK_CHUNK, NULL, SF_FALSE);
  return snd;
err_sf_open:
  fprintf(stderr, "couldn't open \"%s\": %s\n", path, sf_strerror(snd));
  return NULL;
}

//----------

sf_count_t read_from_infile(SNDFILE *snd, unsigned channels, unsigned block_size, float **buf_list) {
  float read_buf[channels];
  sf_count_t f, ret;
  unsigned c;
  for (f = 0; f < block_size; f++) {
    if (!sf_readf_float(snd, read_buf, 1)) break;
    for (c = 0; c < channels; c++) buf_list[c][f] = read_buf[c];
  }
  ret = f;
  for (; f < block_size; f++)
    for (c = 0; c < channels; c++)
      buf_list[c][f] = 0.f;
  return ret;
}

//----------

void write_to_outfile(SNDFILE *snd, unsigned channels, unsigned block_size, float **buf_list) {
  float write_buf[channels];
  sf_count_t f;
  unsigned c;
  for (f = 0; f < block_size; f++) {
    for (c = 0; c < channels; c++)
      write_buf[c] = buf_list[c][f];
    sf_writef_float(snd, write_buf, 1);
  }
}

//----------------------------------------------------------------------
#endif
