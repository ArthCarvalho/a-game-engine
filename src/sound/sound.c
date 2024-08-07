#include "sound/sound.h"

/* MUSIC SEQ DATA */
short seqId;
short vabId;
#define MVOL		127			/* main volume */
#define SVOL		127			/* seq data volume */
char seq_table[SS_SEQ_TABSIZ * 4 * 5]; /* seq data table */

int SoundInit(){
  SsInit ();
  SsSetTableSize (seq_table, 4, 5); /* keep seq data table area */
  
  if(VIDEO_MODE) {
    SsSetTickMode(SS_TICK50);
  } else {
    SsSetTickMode(SS_TICK60);
  }
  
  SsStart2();
}

int LoadSoundTest(u_char * vab_hdata, u_char * vab_bdata, u_long * seq_data){
  vabId = SsVabOpenHead (vab_hdata, -1);
  //printf("VAB HEAD ID: %d addr: %x\n",vabId, vab_hdata);
  if (vabId == -1) {
    //printf ("SsVabOpenHead : failed !!!\n");
    return 1;
  }
  
  if (SsVabTransBody (vab_bdata, vabId) != vabId) {
    //printf ("SsVabTransBody : failed !!!\n");
    return 1;
  }
  SsVabTransCompleted(SS_WAIT_COMPLETED);
  seqId = SsSeqOpen(seq_data, vabId); /* open seq data */
   
  //SsStart();			/* start sound */
  
  
  SsSetMVol(MVOL, MVOL);	/* set main volume */
  SsSeqSetVol(seqId, SVOL, SVOL); /* set seq data volume */
  
  SsSeqPlay(seqId, SSPLAY_PLAY, SSPLAY_INFINITY);
  
  SsUtSetReverbType(6);
  return 0;
}