#ifndef SZAP_H
#define SZAP_H

int zap_to(unsigned int adapter, unsigned int frontend, unsigned int demux,
      unsigned int sat_no, unsigned int freq, unsigned int pol,
      unsigned int sr, unsigned int vpid, unsigned int apid, int sid,
      int dvr, int rec_psi, int bypass, int human_readable);

void set_exit_after_tuning(int do_exit);

#endif
