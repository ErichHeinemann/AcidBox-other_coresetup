static void drums() {
  // float fl_sample = 0.0f;
  // float fr_sample = 0.0f;
    for (int i=0; i < DMA_BUF_LEN; i++){
      Drums.Process( &drums_buf[i], &drums_buf[i+DMA_BUF_LEN] );
    } 
}

static void mixer() { // sum buffers 
    for (int i=0; i < DMA_BUF_LEN; i++) { 
      mix_buf_l[i] =  Synth1.pan*synth_buf[0][i] + Synth2.pan*synth_buf[1][i] + drums_buf[i];
      mix_buf_r[i] =  (1.0f-Synth1.pan)*synth_buf[0][i] + (1.0f-Synth2.pan)*synth_buf[1][i] + drums_buf[i+DMA_BUF_LEN];
    }

}

static void global_fx() { // prepare output
    // we can apply global effects here to mix_buf_l and _r
    for (int i=0; i < DMA_BUF_LEN; i++) { 
       Delay.Process( &mix_buf_l[i], &mix_buf_r[i] );
       Reverb.Process( &mix_buf_l[i], &mix_buf_r[i] );
       mix_buf_l[i] = fclamp(mix_buf_l[i] , -3.0f, 3.0f);
       mix_buf_r[i] = fclamp(mix_buf_r[i] , -3.0f, 3.0f);
    }
}

inline void i2s_output () {  
  for (int i=0; i < DMA_BUF_LEN; i++) { 
//      out_buf._signed[i*2] = 2000 * (3.0f + mix_buf_l[i]);
//      out_buf._signed[i*2+1] = 2000 * (3.0f + mix_buf_r[i]) ;
     
      out_buf._signed[i*2] = 0.333 * 0x8000 * ( mix_buf_l[i]);
      out_buf._signed[i*2+1] = 0.333 * 0x8000 * ( mix_buf_r[i]) ;
  }
  // now out_buf is ready, output

  //i2s_write(i2s_num, out_buf._unsigned, sizeof(out_buf._unsigned), &bytes_written, portMAX_DELAY); // NO-DAC case
  i2s_write(i2s_num, out_buf._signed, sizeof(out_buf._signed), &bytes_written, portMAX_DELAY);
}


/** quick fp clamp
*/
inline float fclamp(float in, float min, float max)
{
    return fmin(fmax(in, min), max);
}
