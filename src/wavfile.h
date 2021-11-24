#ifndef wavfile_included
#define wavfile_included
//----------------------------------------------------------------------

// TODO: class-ify

#include <sndfile.h>

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

SNDFILE* wav_open_input(SF_INFO* info, const char *path) {
  SNDFILE *snd;
  snd = sf_open(path, SFM_READ, info);
  if (!snd) goto err_sf_open;
  return snd;
err_sf_open:
  fprintf(stderr, "couldn't open \"%s\": %s\n", path, sf_strerror(snd));
  return NULL;
}

//----------

void wav_close_input(SNDFILE* wav) {
  if (wav) sf_close(wav);
  wav = NULL;
}

//----------

SNDFILE* wav_open_output(const char *path, int sample_rate, int channels) {
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

void wav_close_output(SNDFILE* wav) {
  if (wav) sf_close(wav);
  wav = NULL;
}

//----------

sf_count_t wav_read_input(SNDFILE *snd, unsigned channels, unsigned block_size, float **buf_list) {
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

void wav_write_output(SNDFILE *snd, unsigned channels, unsigned block_size, float **buf_list) {
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
