#include "mcaRec.h"
#include "device/device.h"
#include "cip_tech_def.h"
#ifdef MSAPANEL
#include "msapanel.h"
#include "dtime.h"
#endif // MSAPANEL

#ifdef PAC_PLCNEXT
#include <cstdlib>
#include <string>
using namespace std::string_literals;
#endif

#include "utf2cp1251.h"

int TRecipeManager::startRecipeBlock = 0;

int TRecipeManager::recipePerLine = 25;

int TRecipeManager::blocksPerRecipe = 4;

int TRecipeManager::recipeNameLength = MAX_REC_NAME_LENGTH - 8;

int TRecipeManager::startRecipeParamsOffset = MAX_REC_NAME_LENGTH;

unsigned char* TRecipeManager::recipeCopyBuffer = nullptr;

TRecipeManager::TRecipeManager( int lineNo ): lineNo(lineNo),
    currentRecipe(0),
    curRecipeStartBlock(0),
    recipeStartAddr(0L)
    {
    defaultfilename = new char[20];
    sprintf(defaultfilename, "line%drec.bin", lineNo);
    recipeMemorySize = blocksPerRecipe * BLOCK_SIZE * recipePerLine;
    recipeMemory = new unsigned char[recipeMemorySize];
    LoadFromFile(defaultfilename);
    lastEvalTime = get_millisec();
    currentRecipeName = new char[recipeNameLength * UNICODE_MULTIPLIER + 1];
    recipeList = new char[(recipeNameLength * UNICODE_MULTIPLIER + 12) * recipePerLine];
    strcpy(recipeList,"");
    ReadMem(startAddr(), recipeNameLength, (unsigned char*)currentRecipeName, true );
    FormRecipeList();
    recipechanged = 0;
    recipechangechecktime = get_millisec();
    }

TRecipeManager::~TRecipeManager()
    {
    SaveRecipeName();
    delete[] currentRecipeName;
    currentRecipeName = nullptr;
    delete[] recipeList;
    recipeList = nullptr;
    SaveToFile(defaultfilename);
    delete[] recipeMemory;
    recipeMemory = nullptr;
    delete[] recipeCopyBuffer;
    recipeCopyBuffer = nullptr;
    delete[] defaultfilename;
    defaultfilename = nullptr;
    }

int TRecipeManager::NextRecipe()
    {
    if (currentRecipe < recipePerLine - 1)
        {
        SaveRecipeName();
        currentRecipe++;
        curRecipeStartBlock += blocksPerRecipe;
        LoadRecipeName();
        return 1;
        }
    else
        {
        return 0;
        }
    }

int TRecipeManager::PrevRecipe()
    {
    if (currentRecipe > 0)
        {
        SaveRecipeName();
        currentRecipe--;
        curRecipeStartBlock -= blocksPerRecipe;
        LoadRecipeName();
        return 1;
        }
    else
        {
        return 0;
        }
    }

void TRecipeManager::EvalRecipe()
    {
    if (get_delta_millisec(lastEvalTime) > 10000L)
        {
        lastEvalTime = get_millisec();
        SaveRecipeName();
        FormRecipeList();
        }
    if (get_delta_millisec(recipechangechecktime) > RECIPE_SAVE_INTERVAL)
        {
        if (recipechanged)
            {
            recipechanged = 0;
            SaveToFile(defaultfilename);
            }
        recipechangechecktime = get_millisec();
        }
    if (getValue(RV_TO_DEFAULTS != 0))
        {
        switch((int)getValue(RV_TO_DEFAULTS))
            {
        case 1:
            ResetRecipeToDefaults(currentRecipe);
#ifdef DEBUG
            printf("Reset recipe %d to defaults",currentRecipe);
#endif
            break;
        case 2:
            CopyRecipe();
            break;
        case 3:
            PasteRecipe();
            break;
        case 4:
            NullifyRecipe();
            break;
            }
        setValue(RV_TO_DEFAULTS, 0);
        }
    }

unsigned long TRecipeManager::startAddr()
    {
    return startAddr(currentRecipe);
    }

unsigned long TRecipeManager::startAddr( int recNo )
    {
    return recipeStartAddr + (recNo * blocksPerRecipe) * BLOCK_SIZE;
    }

float TRecipeManager::getRecipeValue( int recNo, int valueNo )
    {
    unsigned long temp;
    ReadMem(startAddr(recNo) + startRecipeParamsOffset + 4 * valueNo, 4, (unsigned char*)&temp);
    if (temp != 0xFFFFFFFF)
        {
        return *((float*)&temp);
        }
    else
        {
        return -1000;
        }
    }

int TRecipeManager::setRecipeValue( int recNo, int valueNo, float newValue )
    {
    recipechanged = 1;
    recipechangechecktime = get_millisec();
    WriteMem(startAddr(recNo) + startRecipeParamsOffset + 4 * valueNo, 4, (unsigned char*)&newValue);
    return 0;
    }

int TRecipeManager::GetParamsCount()
    {
    return (blocksPerRecipe * BLOCK_SIZE - startRecipeParamsOffset) / 4;
    }

float TRecipeManager::getValue( int valueNo )
    {
    return getRecipeValue(currentRecipe, valueNo);
    }

int TRecipeManager::setValue( int valueNo, float newValue )
    {
#ifdef MSAPANEL
    if (valueNo == RV_IS_USED)
        {
        MsaPanel::UpdateRecipes();
        }
#endif // MSAPANEL
    return setRecipeValue(currentRecipe, valueNo, newValue);
    }

void TRecipeManager::SaveRecipeName()
    {
#ifdef MSAPANEL
    MsaPanel::UpdateRecipes();
#endif // MSAPANEL
    WriteMem(startAddr(), recipeNameLength, (unsigned char*)currentRecipeName,
        true );
    }


int TRecipeManager::LoadRecipeToParams( int recipeNo, int recipeStartPos, int paramsStartPos, int parQuantity, run_time_params_float* par )
    {
    if (recipeNo < recipePerLine && recipeStartPos + parQuantity < GetParamsCount() && (unsigned int)paramsStartPos + (unsigned int)parQuantity < par->get_count())
        {
        for (int i = 0; i < parQuantity; i++)
            {
            par[0][paramsStartPos + i] = getRecipeValue(recipeNo, recipeStartPos + i);
            #ifdef DEBUG
            printf("\n\rsetparamm %d %f\n\r", paramsStartPos + i, par[0][paramsStartPos + i]);
 #endif
            }
        #ifdef DEBUG
        printf("\n\rLoaded recipe %d\n\r",recipeNo);
 #endif
        return 1;
        }
    else
        {
        return 0;
        }
    }

int TRecipeManager::getCurrentRecipe()
    {
    return currentRecipe;
    }

int TRecipeManager::setCurrentRecipe( int recipeNo )
    {
    if (recipeNo >=0 && recipeNo < recipePerLine)
        {
        SaveRecipeName();
        curRecipeStartBlock += blocksPerRecipe * (recipeNo - currentRecipe);
        currentRecipe = recipeNo;
        return 1;
        }
    else
        {
        return 0;
        }
    }

int TRecipeManager::ResetRecipeToDefaults( int recipeNo )
    {
    if (recipeNo >= 0 && recipeNo < recipePerLine)
        {
        setRecipeValue(recipeNo, RV_IS_USED, 0);
        setRecipeValue(recipeNo, RV_TO_DEFAULTS, 0);
        setRecipeValue(recipeNo, RV_V1, 200);
        setRecipeValue(recipeNo, RV_V2, 250);
        setRecipeValue(recipeNo, RV_OBJ_TYPE, 1);
        setRecipeValue(recipeNo, RV_FLOW, 15);
        setRecipeValue(recipeNo, RV_PODP_CIRC, 0);
        setRecipeValue(recipeNo, RV_DELTA_TR, 10);
        setRecipeValue(recipeNo, RV_T_WP, 30);
        setRecipeValue(recipeNo, RV_T_WSP, 30);
        setRecipeValue(recipeNo, RV_T_WKP, 30);
        setRecipeValue(recipeNo, RV_T_WOP, 5);
        setRecipeValue(recipeNo, RV_T_S, 80);
        setRecipeValue(recipeNo, RV_T_K, 70);
        setRecipeValue(recipeNo, RV_T_D, 95);
        setRecipeValue(recipeNo, RV_T_DEZSR, 20);
        setRecipeValue(recipeNo, RV_DOP_V_PR_OP, 1000);
        setRecipeValue(recipeNo, RV_DOP_V_AFTER_S, 750);
        setRecipeValue(recipeNo, RV_DOP_V_AFTER_K, 750);
        setRecipeValue(recipeNo, RV_DOP_V_OK_OP, 500);
        setRecipeValue(recipeNo, RV_RET_STOP, 75);
        setRecipeValue(recipeNo, RV_V_RAB_ML, 30);
        setRecipeValue(recipeNo, RV_V_RET_DEL, 75);
        setRecipeValue(recipeNo, RV_SIGNAL_PUMP_RUNNING, 0);
        setRecipeValue(recipeNo, RV_TM_S, 600);
        setRecipeValue(recipeNo, RV_TM_K, 600);
        setRecipeValue(recipeNo, RV_TM_S_SK, 600);
        setRecipeValue(recipeNo, RV_TM_K_SK, 600);
        setRecipeValue(recipeNo, RV_TM_D, 600);
        setRecipeValue(recipeNo, RV_TM_DEZSR, 600);
        setRecipeValue(recipeNo, RV_TM_DEZSR_INJECT, 2500);
        setRecipeValue(recipeNo, RV_N_RET, 0);
        setRecipeValue(recipeNo, RV_N_UPR, 0);
        setRecipeValue(recipeNo, RV_OS, 0);
        setRecipeValue(recipeNo, RV_OBJ_EMPTY, 0);
        // 5113 because it's a mask for all CIP modes without cold disinfection
        setRecipeValue(recipeNo, RV_PROGRAM_MASK, 5113);
        setRecipeValue(recipeNo, RV_T_RINSING_CLEAN, 5);
        setRecipeValue(recipeNo, RV_V_RINSING_CLEAN, 600);

        setRecipeValue(recipeNo, RV_T_SANITIZER_RINSING, 5);
        setRecipeValue(recipeNo, RV_V_SANITIZER_RINSING, 600);
        setRecipeValue(recipeNo, RV_TM_MAX_TIME_OPORBACHOK, 15);

        setRecipeValue(recipeNo, RV_TM_RET_IS_EMPTY, 8);
        setRecipeValue(recipeNo, RV_V_LL_BOT, 50);
        setRecipeValue(recipeNo, RV_R_NO_FLOW, 5);
        setRecipeValue(recipeNo, RV_TM_R_NO_FLOW, 20);

        setRecipeValue(recipeNo, RV_TM_NO_FLOW_R, 25);
        setRecipeValue(recipeNo, RV_TM_NO_CONC, 30);

        //-PID1
        setRecipeValue(recipeNo, RV_PIDP_Z, 95);
        setRecipeValue(recipeNo, RV_PIDP_k, (float)1.5);
        setRecipeValue(recipeNo, RV_PIDP_Ti, 15);
        setRecipeValue(recipeNo, RV_PIDP_Td, (float)0.2);
        setRecipeValue(recipeNo, RV_PIDP_dt, 500);
        setRecipeValue(recipeNo, RV_PIDP_dmax, 130);
        setRecipeValue(recipeNo, RV_PIDP_dmin, 0);
        setRecipeValue(recipeNo, RV_PIDP_AccelTime, 15);
        setRecipeValue(recipeNo, RV_PIDP_IsManualMode, 0);
        setRecipeValue(recipeNo, RV_PIDP_UManual, 30);
        setRecipeValue(recipeNo, RV_PIDP_Uk, 0);
        //-PID1-!>
        //-PID2
        setRecipeValue(recipeNo, RV_PIDF_Z, 15);
        setRecipeValue(recipeNo, RV_PIDF_k, 0.5);
        setRecipeValue(recipeNo, RV_PIDF_Ti, 10);
        setRecipeValue(recipeNo, RV_PIDF_Td, (float)0.1);
        setRecipeValue(recipeNo, RV_PIDF_dt, 1000);
        setRecipeValue(recipeNo, RV_PIDF_dmax, 66);
        setRecipeValue(recipeNo, RV_PIDF_dmin, 0);
        setRecipeValue(recipeNo, RV_PIDF_AccelTime, 2);
        setRecipeValue(recipeNo, RV_PIDF_IsManualMode, 0);
        setRecipeValue(recipeNo, RV_PIDF_UManual, 15);
        setRecipeValue(recipeNo, RV_PIDF_Uk, 0);
        //-PID2-!>
        setRecipeValue(recipeNo, RV_TM_MAX_TIME_OPORCIP, 120);
        setRecipeValue(recipeNo, RV_SIGNAL_MEDIUM_CHANGE, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_CAUSTIC, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_ACID, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_CIP_IN_PROGRESS, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_CIPEND, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_CIP_READY, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_OBJECT_READY, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_SANITIZER_PUMP, 0);
        setRecipeValue(recipeNo, RV_RESUME_CIP_ON_SIGNAL, 1);
        setRecipeValue(recipeNo, RV_SIGNAL_PUMP_CONTROL, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_DESINSECTION, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_OBJECT_PAUSE, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_PUMP_CAN_RUN, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_PUMP_CONTROL_FEEDBACK, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_RET_PUMP_SENSOR, 0);
        setRecipeValue(recipeNo, RV_RET_PUMP_SENSOR_DELAY, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_IN_CIP_READY, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_CIPEND2, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_CAN_CONTINUE, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_WATER, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_PRERINSE, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_INTERMEDIATE_RINSE, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_POSTRINSE, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_PUMP_STOPPED, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_FLOW_TASK, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_TEMP_TASK, 0);
        setRecipeValue(recipeNo, RV_SIGNAL_WASH_ABORTED, 0);
        setRecipeValue(recipeNo, RV_PRESSURE_CONTROL, 0);
        setRecipeValue(recipeNo, RV_DONT_USE_WATER_TANK, 0);
        setRecipeValue(recipeNo, RV_PIDP_MAX_OUT, 0);
        setRecipeValue(recipeNo, RV_PIDF_MAX_OUT, 0);
        setRecipeValue( recipeNo, RV_WATCHDOG, 0 );

        for (int i = RV_RESERV_START; i <= RV_LASTVALVEOFF; i++)
            {
            setRecipeValue(recipeNo, i, 0);
            }
        return 1;
        }
    else
        {
        return 0;
        }
    }

void TRecipeManager::FormRecipeList()
    {
    strcpy(recipeList, "");
    char tmprecipename[MAX_REC_NAME_LENGTH * UNICODE_MULTIPLIER];
    for (int i = 0; i < recipePerLine; i++)
        {
        if (getRecipeValue(i, RV_IS_USED) != 0)
            {
            ReadMem(startAddr( i ), recipeNameLength, (unsigned char*)tmprecipename, true );
            sprintf(recipeList + strlen(recipeList), "%d##%s||", i + 1, tmprecipename);
            }

        }
    }

void TRecipeManager::LoadRecipeName()
    {
    ReadMem(startAddr(), recipeNameLength, (unsigned char*)currentRecipeName, true );
    }

int TRecipeManager::ToRecipe( int recNo )
    {
    if (recNo >= 0 && recNo < recipePerLine)
        {
        SaveRecipeName();
        curRecipeStartBlock += blocksPerRecipe * (recNo - currentRecipe);
        currentRecipe = recNo;
        LoadRecipeName();
        return 1;
        }
    else
        {
        return 0;
        }
    }

int TRecipeManager::getRecipeName( int recNO, char* recName )
    {
    return ReadMem(startAddr(recNO), recipeNameLength, (unsigned char*)recName, true );
    }

int TRecipeManager::OnRecipeDevices( int recipeNo, int msaline /*= 1*/ )
{
    unsigned int devNo;
    char devName[25];
    device* dev;
    int i;
    int errflag = 0;
    if (recipeNo < 0)
        {
        return 0;
        }
    for (i = RV_FIRSTVALVEON; i<= RV_LASTVALVEON; i++)
        {
        devNo = (unsigned long)getRecipeValue(recipeNo, i);
        if (devNo > 0)
            {
            sprintf(devName,"LINE%dV%u", msaline, devNo);
            dev = (device*)(V(devName));
            if (dev->get_serial_n() > 0)
                {
                dev->on();
                }
            else
                {
                dev = DEVICE(devNo);
                if (dev->get_serial_n() > 0)
                    {
                    dev->on();
                    }
                else
                    {
#ifdef DEBUG
                    printf("\n\rOn recipe devices. Valve %u from param %d not found in device list\n\r", devNo, i);
#endif
                    errflag = 1;
                    }
                }
            }
        }
    for (i = RV_FIRSTVALVEOFF; i<= RV_LASTVALVEOFF; i++)
        {
        devNo = (unsigned long)getRecipeValue(recipeNo, i);
        if (devNo > 0)
            {
            sprintf(devName,"LINE%dV%u", msaline, devNo);
            dev = (device*)(V(devName));
            if (dev->get_serial_n() > 0)
                {
                dev->off();
                }
            else
                {
                dev = DEVICE(devNo);
                if (dev->get_serial_n() > 0)
                    {
                    dev->off();
                    }
                else
                    {
#ifdef DEBUG
                    printf("\n\rOff recipe devices. Valve %u from param %d not found in device list\n\r", devNo, i);
#endif
                    errflag = 1;
                    }
                }
            }
        }
    return errflag;
    }

int TRecipeManager::OffRecipeDevices( int recipeNo, int msaline /*= 1*/ )
    {
    unsigned int devNo;
    char devName[25];
    device* dev;
    int i;
    int errflag = 0;
    if (recipeNo < 0)
        {
        return 0;
        }
    for (i = RV_FIRSTVALVEON; i<= RV_LASTVALVEON; i++)
        {
        devNo = (unsigned long)getRecipeValue(recipeNo, i);
        if (devNo > 0)
            {
            sprintf(devName,"LINE%dV%u", msaline, devNo);
            dev = (device*)(V(devName));
            if (dev->get_serial_n() > 0)
                {
                dev->off();
                }
            else
                {
                dev = DEVICE(devNo);
                if (dev->get_serial_n() > 0)
                    {
                    dev->off();
                    }
                else
                    {
#ifdef DEBUG
                    printf("\n\rOff recipe devices. Valve %u from param %d not found in device list\n\r", devNo, i);
#endif
                    errflag = 1;
                    }
                }
            }
        }
    return errflag;
    }

void TRecipeManager::CopyRecipe()
    {
    if (recipeCopyBuffer != nullptr)
        {
        delete[] recipeCopyBuffer;
        }
    recipeCopyBuffer = new unsigned char[BLOCK_SIZE * blocksPerRecipe];
    ReadMem(startAddr(), BLOCK_SIZE * blocksPerRecipe, recipeCopyBuffer);
    }

void TRecipeManager::PasteRecipe()
    {
    if (recipeCopyBuffer != nullptr)
        {
        WriteMem(startAddr(), BLOCK_SIZE * blocksPerRecipe, recipeCopyBuffer);
        LoadRecipeName();
        }
    }

void TRecipeManager::NullifyRecipe()
    {
    unsigned char* tempbuff = new unsigned char[BLOCK_SIZE * blocksPerRecipe];
    memset(tempbuff, 0, BLOCK_SIZE * blocksPerRecipe);
    WriteMem(startAddr(), BLOCK_SIZE * blocksPerRecipe, tempbuff);
    delete [] tempbuff;
    tempbuff = nullptr;
    LoadRecipeName();
    }

int TRecipeManager::ReadMem( unsigned long startaddr, unsigned long length,
    unsigned char* buf, bool is_string )
    {
    if ( is_string )
        {
        auto tmp = new char[ length + 1 ];
        memcpy( tmp, recipeMemory + startaddr, length );
        tmp[ length ] = '\0';
        convert_windows1251_to_utf8( (char*)buf, tmp );
        delete[] tmp;
        }
    else
        {
        memcpy( buf, recipeMemory + startaddr, length );
        }

    return 0;
    }

int TRecipeManager::WriteMem( unsigned long startaddr, unsigned long length,
    unsigned char* buf, bool is_string )
    {
    if ( is_string )
        {
        auto tmp = new char[ length ] {};
        convert_utf8_to_windows1251( (char*)buf, tmp,
            length * UNICODE_MULTIPLIER, length );
        memcpy( recipeMemory + startaddr, tmp, length );
        delete[] tmp;
        }
    else
        {
        memcpy( recipeMemory + startaddr, buf, length );
        }

    return 0;
    }

int TRecipeManager::SaveToFile( const char* filename )
    {
#ifdef DEBUG
    printf( "Saving recipes to file %s\n", filename );
#endif // DEBUG
    FILE* memFile = nullptr;
    char fname[ 50 ];
#ifdef PAC_PLCNEXT
    sprintf( fname, "/opt/main/%s", filename );
#else
    sprintf( fname, "%s", filename );
#endif // PAC_PLCNEXT
    memFile = fopen( fname, "r+b" );
    if ( nullptr == memFile )
        {
        memFile = fopen( fname, "w+b" );
        }
    if ( memFile )
        {
        fseek( memFile, 0, SEEK_SET );
        fwrite( recipeMemory, 1, recipeMemorySize, memFile );
        fclose( memFile );
#ifdef PAC_PLCNEXT
        std::string syscommand = "chmod 777 "s + fname;
        system( syscommand.c_str() );
#endif
        }
    return 0;
    }

int TRecipeManager::LoadFromFile( const char* filename )
    {
    FILE* memFile = nullptr;
    char fname[50];
#ifdef PAC_PLCNEXT
    sprintf(fname, "/opt/main/%s", filename);
#else
    sprintf( fname, "./%s", filename );
#endif // PAC_PLCNEXT
    memset(recipeMemory, 0, recipeMemorySize);
    memFile = fopen(fname, "r+b");
    if (memFile)
        {
        size_t result = fread(recipeMemory, 1, recipeMemorySize, memFile);
        fclose(memFile);
        if (0 == result) return 1;
        }
    return 0;
    }


int TMediumRecipeManager::startRecipeBlock = 0;

int TMediumRecipeManager::recipePerLine = 10;

int TMediumRecipeManager::blocksPerRecipe = 1;

int TMediumRecipeManager::recipeNameLength = MAX_REC_NAME_LENGTH - 2;

int TMediumRecipeManager::startRecipeParamsOffset = MAX_REC_NAME_LENGTH;

unsigned char* TMediumRecipeManager::recipeCopyBuffer = nullptr;

TMediumRecipeManager::TMediumRecipeManager(MediumTypes mType) :
mediumType(mType),
currentRecipe(0),
curRecipeStartBlock(0),
recipeStartAddr(0L)
{
    defaultfilename = new char[20];
    sprintf(defaultfilename, "medium%drec.bin", mType);
    recipeMemorySize = blocksPerRecipe * BLOCK_SIZE * recipePerLine;
    recipeMemory = new unsigned char[recipeMemorySize];
    LoadFromFile(defaultfilename);
    lastEvalTime = get_millisec();
    currentRecipeName = new char[recipeNameLength * UNICODE_MULTIPLIER ];
    recipeList = new char[(recipeNameLength * UNICODE_MULTIPLIER + 6) * recipePerLine];
    strcpy(recipeList, "");
    ReadMem(startAddr(), recipeNameLength, (unsigned char*)currentRecipeName);
    FormRecipeList();
    recipechanged = 0;
    recipechangechecktime = get_millisec();
}

TMediumRecipeManager::~TMediumRecipeManager()
{
    SaveRecipeName();
    delete[] currentRecipeName;
    currentRecipeName = nullptr;
    delete[] recipeList;
    recipeList = nullptr;
    SaveToFile(defaultfilename);
    delete[] recipeMemory;
    recipeMemory = nullptr;
    delete[] recipeCopyBuffer;
    recipeCopyBuffer = nullptr;
    delete[] defaultfilename;
    defaultfilename = nullptr;
}

int TMediumRecipeManager::NextRecipe()
{
    if (currentRecipe < recipePerLine - 1)
    {
        SaveRecipeName();
        currentRecipe++;
        curRecipeStartBlock += blocksPerRecipe;
        LoadRecipeName();
        return 1;
    }
    else
    {
        return 0;
    }
}

int TMediumRecipeManager::PrevRecipe()
{
    if (currentRecipe > 0)
    {
        SaveRecipeName();
        currentRecipe--;
        curRecipeStartBlock -= blocksPerRecipe;
        LoadRecipeName();
        return 1;
    }
    else
    {
        return 0;
    }
}

void TMediumRecipeManager::EvalRecipe()
{
    if (get_delta_millisec(lastEvalTime) > 10000L)
    {
        lastEvalTime = get_millisec();
        SaveRecipeName();
        FormRecipeList();
    }
    if (get_delta_millisec(recipechangechecktime) > RECIPE_SAVE_INTERVAL)
    {
        if (recipechanged)
        {
            recipechanged = 0;
            SaveToFile(defaultfilename);
        }
        recipechangechecktime = get_millisec();
    }
    if (getValue(RV_TO_DEFAULTS != 0))
    {
        switch ((int)getValue(RV_TO_DEFAULTS))
        {
        case 1:
            ResetRecipeToDefaults(currentRecipe);
#ifdef DEBUG
            printf("Reset recipe %d to defaults", currentRecipe);
#endif
            break;
        case 2:
            CopyRecipe();
            break;
        case 3:
            PasteRecipe();
            break;
        case 4:
            NullifyRecipe();
            break;
        }
        setValue(RV_TO_DEFAULTS, 0);
    }
}

unsigned long TMediumRecipeManager::startAddr()
{
    return startAddr(currentRecipe);
}

unsigned long TMediumRecipeManager::startAddr(int recNo)
{
    return recipeStartAddr + (recNo * blocksPerRecipe) * BLOCK_SIZE;
}

float TMediumRecipeManager::getRecipeValue(int recNo, int valueNo)
{
    unsigned long temp;
    ReadMem(startAddr(recNo) + startRecipeParamsOffset + 4 * valueNo, 4, (unsigned char*)&temp);
    if (temp != 0xFFFFFFFF)
    {
        return *((float*)&temp);
    }
    else
    {
        return -1000;
    }
}

int TMediumRecipeManager::setRecipeValue(int recNo, int valueNo, float newValue)
{
    recipechanged = 1;
    recipechangechecktime = get_millisec();
    WriteMem(startAddr(recNo) + startRecipeParamsOffset + 4 * valueNo, 4, (unsigned char*)&newValue);
    return 0;
}

int TMediumRecipeManager::GetParamsCount()
{
    return (blocksPerRecipe * BLOCK_SIZE - startRecipeParamsOffset) / 4;

}

float TMediumRecipeManager::getValue(int valueNo)
{
    return getRecipeValue(currentRecipe, valueNo);
}

int TMediumRecipeManager::setValue(int valueNo, float newValue)
{
    return setRecipeValue(currentRecipe, valueNo, newValue);
}

void TMediumRecipeManager::SaveRecipeName()
{
    WriteMem(startAddr(), recipeNameLength, (unsigned char*)currentRecipeName );
}


int TMediumRecipeManager::LoadRecipeToParams(int recipeNo, saved_params<float, true> par)
{
    switch (mediumType)
        {
        case MT_CAUSTIC:
            par[P_CZAD_S] = getRecipeValue(recipeNo, RV_P_CZAD);
            par[P_CMIN_S] = getRecipeValue(recipeNo, RV_P_CMIN);
            par[P_CKANAL_S] = getRecipeValue(recipeNo, RV_P_CKANAL);
            par[P_ALFS] = getRecipeValue(recipeNo, RV_P_ALF);
            par[P_K_S] = getRecipeValue(recipeNo, RV_P_K);
            par[P_RO_S] = getRecipeValue(recipeNo, RV_P_RO);
            par[P_CONC25S] = getRecipeValue(recipeNo, RV_P_CONC25);
            par[P_MS25S] = getRecipeValue(recipeNo, RV_P_MS25);
            par[P_CAUSTIC_TYPE] = getRecipeValue(recipeNo, RV_P_TYPE);
            break;
        case MT_ACID:
            par[P_CZAD_K] = getRecipeValue(recipeNo, RV_P_CZAD);
            par[P_CMIN_K] = getRecipeValue(recipeNo, RV_P_CMIN);
            par[P_CKANAL_K] = getRecipeValue(recipeNo, RV_P_CKANAL);
            par[P_ALFK] = getRecipeValue(recipeNo, RV_P_ALF);
            par[P_K_K] = getRecipeValue(recipeNo, RV_P_K);
            par[P_RO_K] = getRecipeValue(recipeNo, RV_P_RO);
            par[P_CONC25K] = getRecipeValue(recipeNo, RV_P_CONC25);
            par[P_MS25K] = getRecipeValue(recipeNo, RV_P_MS25);
            par[P_ACID_TYPE] = getRecipeValue(recipeNo, RV_P_TYPE);
            break;
        default:
            break;
        }
    par.save_all();
    return 0;
}

int TMediumRecipeManager::getCurrentRecipe()
{
    return currentRecipe;
}

int TMediumRecipeManager::setCurrentRecipe(int recipeNo)
{
    if (recipeNo >= 0 && recipeNo < recipePerLine)
    {
        SaveRecipeName();
        curRecipeStartBlock += blocksPerRecipe * (recipeNo - currentRecipe);
        currentRecipe = recipeNo;
        return 1;
    }
    else
    {
        return 0;
    }
}

int TMediumRecipeManager::ResetRecipeToDefaults(int recipeNo)
{
    if (recipeNo >= 0 && recipeNo < recipePerLine)
    {
        setRecipeValue(recipeNo, RV_IS_USED, 0);
        setRecipeValue(recipeNo, RV_TO_DEFAULTS, 0);
        return 1;
    }
    else
    {
        return 0;
    }
}

void TMediumRecipeManager::FormRecipeList()
{
    strcpy(recipeList, "");
    char tmprecipename[MAX_REC_NAME_LENGTH * UNICODE_MULTIPLIER];
    for (int i = 0; i < recipePerLine; i++)
    {
        if (getRecipeValue(i, RV_IS_USED) != 0)
        {
            ReadMem(startAddr(i), recipeNameLength, (unsigned char*)tmprecipename, true);
            sprintf(recipeList + strlen(recipeList), "%d##%s||", i + 1, tmprecipename);
        }

    }
}

void TMediumRecipeManager::LoadRecipeName()
{
    ReadMem(startAddr(), recipeNameLength, (unsigned char*)currentRecipeName, true);
}

int TMediumRecipeManager::ToRecipe(int recNo)
{
    if (recNo >= 0 && recNo < recipePerLine)
    {
        SaveRecipeName();
        curRecipeStartBlock += blocksPerRecipe * (recNo - currentRecipe);
        currentRecipe = recNo;
        LoadRecipeName();
        return 1;
    }
    else
    {
        return 0;
    }
}

int TMediumRecipeManager::getRecipeName(int recNO, char* recName)
{
    return ReadMem(startAddr(recNO), recipeNameLength, (unsigned char*)recName);
}

void TMediumRecipeManager::CopyRecipe()
{
    if (recipeCopyBuffer != nullptr)
    {
        delete[] recipeCopyBuffer;
    }
    recipeCopyBuffer = new unsigned char[BLOCK_SIZE * blocksPerRecipe];
    ReadMem(startAddr(), BLOCK_SIZE * blocksPerRecipe, recipeCopyBuffer);
}

void TMediumRecipeManager::PasteRecipe()
{
    if (recipeCopyBuffer != nullptr)
    {
        WriteMem(startAddr(), BLOCK_SIZE * blocksPerRecipe, recipeCopyBuffer);
        LoadRecipeName();
    }
}

void TMediumRecipeManager::NullifyRecipe()
{
    unsigned char* tempbuff = new unsigned char[BLOCK_SIZE * blocksPerRecipe];
    memset(tempbuff, 0, BLOCK_SIZE * blocksPerRecipe);
    WriteMem(startAddr(), BLOCK_SIZE * blocksPerRecipe, tempbuff);
    delete[] tempbuff;
    tempbuff = nullptr;
    LoadRecipeName();
}

int TMediumRecipeManager::ReadMem( unsigned long startaddr, unsigned long length,
    unsigned char* buf, bool is_string )
    {
    if ( is_string )
        {
        auto tmp = new char[ length + 1 ];
        memcpy( tmp, recipeMemory + startaddr, length );
        tmp[ length ] = '\0';
        convert_windows1251_to_utf8( (char*)buf, tmp );
        delete[] tmp;
        }
    else
        {
        memcpy( buf, recipeMemory + startaddr, length );
        }

    return 0;
    }

int TMediumRecipeManager::WriteMem( unsigned long startaddr,
    unsigned long length, unsigned char* buf, bool is_string )
    {
    if ( is_string )
        {
        auto tmp = new char[ length ] {};
        convert_utf8_to_windows1251( (char*)buf, tmp,
            length * UNICODE_MULTIPLIER, length );
        memcpy( recipeMemory + startaddr, tmp, length );
        delete[] tmp;
        }
    else
        {
        memcpy( recipeMemory + startaddr, buf, length );
        }

    return 0;
    }

int TMediumRecipeManager::SaveToFile( const char* filename )
    {
#ifdef DEBUG
    printf( "Saving recipes to file %s\n", filename );
#endif // DEBUG
    FILE* memFile = nullptr;
    char fname[ 50 ];
#ifdef PAC_PLCNEXT
    sprintf( fname, "/opt/main/%s", filename );
#else
    sprintf( fname, "%s", filename );
#endif // PAC_PLCNEXT
    memFile = fopen( fname, "r+b" );
    if ( nullptr == memFile )
        {
        memFile = fopen( fname, "w+b" );
        }
    if ( memFile )
        {
        fseek( memFile, 0, SEEK_SET );
        fwrite( recipeMemory, 1, recipeMemorySize, memFile );
        fclose( memFile );
#ifdef PAC_PLCNEXT
        std::string syscommand = "chmod 777 "s + fname;
        system( syscommand.c_str() );
#endif
        }
    return 0;
    }

int TMediumRecipeManager::LoadFromFile(const char* filename)
{
    FILE* memFile = nullptr;
    char fname[50];
    memset(recipeMemory, 0, recipeMemorySize);
#ifdef PAC_PLCNEXT
    sprintf(fname, "/opt/main/%s", filename);
#else
    sprintf(fname, "%s", filename);
#endif // PAC_PLCNEXT
    memFile = fopen(fname, "r+b");
    if (memFile)
    {
        size_t result = fread(recipeMemory, 1, recipeMemorySize, memFile);
        fclose(memFile);
        if (0 == result) return 1;
    }
    return 0;
}
