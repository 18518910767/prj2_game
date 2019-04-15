#include "Aprotocol.h"
#include <stdlib.h>

bool RawData::AppendData(const RawData *pstData)
{
    bool bRet = false;
    unsigned char *pucTmpData = (unsigned char *)calloc(1UL, pstData->iLength + this->iLength);

    if (NULL != pucTmpData)
    {
        memcpy(pucTmpData, this->pucData, this->iLength);
        memcpy(pucTmpData + this->iLength, pstData->pucData, pstData->iLength);
        this->iLength += pstData->iLength;
        if (NULL != this->pucData)
        {
            free(this->pucData);
        }
        this->pucData = pucTmpData;
        bRet = true;
    }
    

    return bRet;
}

/*���ò����е����ݿ���������*/
bool RawData::SetData(unsigned char * _pucData, int _iLength)
{
	bool bRet = false;

	/*���ݲ���ָ��������*/
	unsigned char *pucTmp = (unsigned char *)calloc(1UL, _iLength);
	if (NULL != pucTmp)
	{
		memcpy(pucTmp, _pucData, _iLength);
		/*ԭ�������ݣ��ͷ�*/
		if (NULL != this->pucData)
		{
			free(this->pucData);
			this->pucData = NULL;
		}
		this->pucData = pucTmp;
		this->iLength = _iLength;
		bRet = true;
	}
	
	return bRet;
}

RawData::~RawData()
{
    if (NULL != this->pucData)
    {
        free(this->pucData);
        this->pucData = NULL;
    }

    this->iLength = 0;
}

Aprotocol::Aprotocol()
{

}

Aprotocol::~Aprotocol()
{
}
