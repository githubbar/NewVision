/*
Author : Nish
EMail : nishforever@vsnl.com
*/

#include "stdafx.h"
#include "Expire.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

CExpire::CExpire(const char*  ProgName,const char* KeyName,UINT Num,UINT ExpireType)
{	
	m_first=true;
	m_ExpireType=ExpireType;
	m_count=Num;
	CreateProgKey(ProgName,KeyName);
	UINT rcount;	
	GetSystemDirectory(m_systemfolder,MAX_PATH);
	strcpy(m_filename,m_systemfolder);
	if(m_filename[strlen(m_filename)-1]!='\\')
		strcat(m_filename,"\\");	
	strcat(m_filename,KeyName);
	strcat(m_filename,".dll");
	m_filefound=LocateFile();
	switch (m_ExpireType)
	{
	case TYPERUNS:		
		if(GetRunCount(rcount))
		{
			if(!m_filefound)
			{			
				SetRunCount();
				m_first=false;
				CreateProtectFile();
			}
		}
			
		else
		{		
			if(rcount>0 && rcount<=m_count)							
			{
				SetRunCount(rcount-1);
				m_first=false;
			}		
		}
		break;
	case TYPEDAYS:
		
		if(GetDayCount(rcount))
		{
			if(!m_filefound)
			{			
				SetDayCount();
				m_first=false;
				CreateProtectFile();
			}
		}
			
		else
		{	
			
			if(rcount>0 && rcount<=m_count)							
			{				
				m_first=false;
			}		
		}

		break;
	}


}

CExpire::~CExpire()
{
	RegCloseKey(m_hKey);
}

bool CExpire::CreateProgKey(const char*  ProgName, const char* KeyName)
{
	char *SubKey = new char [strlen(ProgName) +strlen(KeyName)+300];
	strcpy(SubKey,"CLSID\\");
	strcat(SubKey,ProgName);
	strcat(SubKey,"\\");
	strcat(SubKey,KeyName);
	HKEY hKey;
	DWORD dwDisposition;
	LONG ret;
	ret=RegCreateKeyEx(HKEY_CLASSES_ROOT,SubKey,0,"",REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,NULL,&hKey,&dwDisposition);
	m_hKey=hKey;
	delete SubKey;
	if(ret==ERROR_SUCCESS)
		return false;
	else
		return true;
}

bool CExpire::GetRunCount(UINT& count)
{
	LONG ret;
	char data[128];
	DWORD size=sizeof data;
	ret=RegQueryValueEx(m_hKey,"windll",0,NULL,(BYTE*)data,&size);
	if(ret==ERROR_SUCCESS)
	{
		DeObfuscate(data,count);
		
		return false;
	}
	else
		return true;
}

bool CExpire::SetRunCount()
{
	LONG ret;
	char str[128];	
	Obfuscate(m_count-1,str);
	DWORD size=strlen(str)+1;
	ret=RegSetValueEx(m_hKey,"windll",0,REG_SZ,(BYTE*)str,size);
	if(ret==ERROR_SUCCESS)
		return false;
	else
		return true;
}

bool CExpire::SetRunCount(UINT count)
{
	LONG ret;
	char str[128];	
	Obfuscate(count,str);
	DWORD size=strlen(str)+1;
	ret=RegSetValueEx(m_hKey,"windll",0,REG_SZ,(BYTE*)str,size);
	if(ret==ERROR_SUCCESS)
		return false;
	else
		return true;
}


bool CExpire::Obfuscate(UINT num, char *str)
{	
	char tmpstr[128];
	srand((unsigned int)time(NULL));
	UINT seed1,seed2,seed3;
	seed1=rand();
	seed2=rand();
	seed3=abs((int)(seed1-seed2));
	sprintf(tmpstr,"%d.%d.%d",seed1,num+seed3,seed2);
	strcpy(str,tmpstr);
	return false;
}

bool CExpire::DeObfuscate(char *str, UINT &num)
{
	char tmpstr[128];
	strcpy(tmpstr,str);
	UINT seed1,seed2,seed3;
	char *p1=strchr(tmpstr,'.');	
	if(!p1)
	{
		return true;
	}
	*p1=0;
	seed1=atoi(tmpstr);
	p1++;
	char *p2=strchr(p1,'.');
	if(!p2)
	{
		return true;
	}
	*p2=0;
	int tmpNum=atoi(p1);
	p2++;
	seed2=atoi(p2);
	seed3=abs((int)(seed1-seed2));
	num=tmpNum-seed3;
	return false;
}

bool CExpire::SetDayCount()
{
	LONG ret;
	char str[128];	
	UINT TodaySecs=time(NULL);
	TodaySecs+=(m_count)*24*3600;	
	Obfuscate(TodaySecs,str);
	DWORD size=strlen(str)+1;
	ret=RegSetValueEx(m_hKey,"windll",0,REG_SZ,(BYTE*)str,size);
	if(ret==ERROR_SUCCESS)
		return false;
	else
		return true;
}


bool CExpire::GetDayCount(UINT& count)
{
	LONG ret;
	char data[128];
	DWORD size=sizeof data;
	ret=RegQueryValueEx(m_hKey,"windll",0,NULL,(BYTE*)data,&size);
	if(ret==ERROR_SUCCESS)
	{
		DeObfuscate(data,count);
		UINT TodaySecs=time(NULL);		
		UINT tmpcount=count;
		count=(count-TodaySecs)/(24*3600);
		if((tmpcount-TodaySecs)%(24*3600))
			count++;
		return false;
	}
	else
		return true;
}

bool CExpire::HasExpired()
{
	if(m_first)
		RegDeleteValue(m_hKey,"windll");
	return m_first;
}

bool CExpire::LocateFile()
{

	WIN32_FIND_DATA tmpWIN32_FIND_DATA;
	HANDLE hFile=FindFirstFile(m_filename,&tmpWIN32_FIND_DATA);
	if (hFile==INVALID_HANDLE_VALUE)
		return false;
	else
	{
		
		FindClose(hFile);
		return true;
	}
}

bool CExpire::CreateProtectFile()
{
	char buff[32];
	buff[31]=0;
	bool ret=false;
	FILE* fp=fopen(m_filename,"w");
	fprintf(fp,buff);
	fclose(fp);
	return ret;
}

UINT CExpire::GetRunsLeft()
{
	UINT n=0;
	GetRunCount(n);
	return n;
}

UINT CExpire::GetDaysLeft()
{
	UINT n=0;
	GetDayCount(n);
	return n;	
}
