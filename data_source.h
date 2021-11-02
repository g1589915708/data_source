#pragma once

#ifndef _DATA_CORE_H_
#define _DATA_CORE_H_

#define _STATIC_

#ifdef _STATIC_
#define _DATA_CORE_ __attribute__((cdecl))
#else
#define _DATA_CORE_ __declspec(dllexport)
#endif

#include <cJSON/cJSON.h>
#include <eredis/eredis.h>

typedef struct _CONFIG_SOURCE_ config_source;
struct _CONFIG_SOURCE_{
	char path[128];			/* 数据的路径 */
	char name[64];			/* 数据的名称 */
	char date[32];			/*  */
	char source[128];		/*  */
	char alternate[128];		/*  */
	char relation[8][128];		/*  */
	cJSON * c_source;
};

typedef struct _DATA_SOURCE_ data_source;
struct _DATA_SOURCE_{
	/*-- 数据的基础 --*/
	char (*name)[64];			/* 解析的名字 */
	int * type;				/* 解析的类型 */
	char (*alias)[32];			/* 程序的别名 */
	char (*explain)[128];			/* 程序的解释 */
	short status;				/*  */
	char date[32];				/*  */
	/*-- 程序需要的数据 --*/
	config_source config;
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	/*-- 配置 --*/
	eredis_t * _DATA_CORE_ init_engine();
	/*-- 扫描配置 --*/
	void _DATA_CORE_ init_data_sanner();
	/*-- 解析json数据源,通过路径和名字解析数据 --*/
	data_source * _DATA_CORE_ analysis_json_sdata(const char * path,const char * name);
	/*-- 读取数据 --*/
	cJSON * _DATA_CORE_ read_data_source(data_source ** sdata);
	/*-- 保存数据 --*/
	char * _DATA_CORE_ save_data(cJSON * data);
	/*-- 保存数据源 --*/
	char * _DATA_CORE_ save_sdata(data_source * sdata,cJSON * data);
	/*-- 删除数据源 --*/
	int _DATA_CORE_ del_data(const char * path);
	/*-- 查找数据源 --*/
	data_source * _DATA_CORE_ find_sdata(const char * path,int flag);
	/*-- 查找数据 --*/
	cJSON * _DATA_CORE_ find_data(const char * path,int flag);
	/*-- 修改数据源 --*/
	data_source * _DATA_CORE_ update_sdata(const char * path,int flag,data_source * data);
	/*-- 修改数据 --*/
	cJSON * _DATA_CORE_ update_data(const char * path,int flag,cJSON * data);
	
	
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !_DATA_CORE_H_
