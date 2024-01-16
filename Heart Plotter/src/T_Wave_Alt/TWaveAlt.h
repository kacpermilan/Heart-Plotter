#include "iTWaveAlt.h"
#include "../Common_Types/CommonTypes.h"
#include "../Waves/iWaves.h"
#include <vector>
#include <memory>

class TWaveAlt : public iTWaveAlt
{
public:
    TWaveAlt() = default;

    OperationStatus detect_T_wave_alternan(std::vector<DataPoint> signal,
                                                  std::unique_ptr<iWaves> waves) override;

};


