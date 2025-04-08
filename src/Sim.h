#ifndef GEN_H_
#define GEN_H_

class Sim {
    public:
        Sim();
        const float generarSenyalECG();
        const float generarSenyalRES();

        volatile float temps;
        float dt;
        float rr_interval;
        float rr_variability;
        float fs;

    private:
        // Constants per simular l'ECG
        //static const float _fs = 500.0;
        const float _duration = 10.0;
        const float _heart_rate = 60.0;

        // Paràmetres de la forma de l'ECG
        const float _P_amp = 0.1;
        const float _QRS_amp = 1.0;
        const float _T_amp = 0.3;

        const float _P_duration = 0.1;
        const float _QRS_duration = 0.1;
        const float _T_duration = 0.2;

        // Paràmetres de la forma de la respiració        
        const float resp_freq = 0.25; // Hz (15 respiracions per minut)
        const float resp_ampl = 0.5;  // mV
        const float resp_offset = 3.0; // mV


        float gaussian_wave(float t, float amplitude, float duration);
        float qrs_wave_with_negatives(float t, float amplitude, float duration);
};

#endif  // GEN_H_