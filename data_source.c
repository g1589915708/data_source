#include "data_source.h"
#include <malloc.h>
#include <pthread.h>

#define SPATH_DATA "../sdata/data_config.data"
#define SNAME "name"
#define STYPE "type"
#define SPATH "path"
#define SDATE "date"
#define SARRAY "array"
#define SALIAS "alias"
#define SEXPLAIN "explain"


static char * open_file(const char * pt);
static void _init_data_sanner(int t);

eredis_t * _DATA_CORE_ init_engine(){
	static eredis_t * ert = NULL;
	if(!ert){
		ert = eredis_new();
		eredis_run_thr( ert );
		if(ert != NULL) { printf("nice!!\n"); }
		else { exit(0); }
		/* Set timeout - default 5000ms */
		eredis_timeout( ert, 200 );
		/* Set max readers - default 10 */
		eredis_r_max( ert, 32 );
		/* Set retry for reader - default 1 */
		eredis_r_retry( ert, 2 );
		eredis_host_add( ert, "127.0.0.1", 6379 );
		/* via configuration file - one line per host:port */
		eredis_host_file( ert, "../config/hosts.conf" );
	}
	return ert;
}
/*-- [{"date":"2021-01-01 00:00:00","path":"../test.data"}] --*/
void _DATA_CORE_ init_data_sanner(){
	void* (*fun)(void*) = &_init_data_sanner;
	pthread_t  *thread;
	int  rc,t = 10000001;
	rc = pthread_create(&thread, NULL, fun, t);

	if  (rc){
             printf ( "ERROR:line:%d,function %s\t; return code is %d\n" ,__LINE__,__FUNCTION__, rc);
             return;
	}
	printf("ok");
	pthread_join( thread,NULL);
	
}







char * open_file(const char * pt){
	FILE * fp = NULL;
	long int len = 0;
	char * res = NULL;
	if((fp = fopen(pt,"rb"))==NULL){
		printf("open %s file failed!!!\n",pt);
		return NULL;
	}
	fseek(fp,0,SEEK_END);
	len = ftell(fp);
	fseek(fp,0,SEEK_SET);
	res = malloc(sizeof(char)*len+1);
	res[len] = '\0';
	fread(res,1,len,fp);
	return res;
}

void _init_data_sanner(int t){
	printf("%d\n",t);
	cJSON * root = NULL;
	cJSON * item = NULL;
	cJSON * name = NULL;
	cJSON * type = NULL;
	cJSON * path = NULL;
	cJSON * date = NULL;
	cJSON * source = NULL;
	cJSON * alternate  = NULL;
	cJSON * explain = NULL;
	cJSON * group = NULL;
	int size = 0;
	int i = 0;
	char * data = NULL;
	int _t = t;
	int _w = 1;
	int set_time = 0;
	eredis_t *ert = NULL;
	eredis_reader_t *r = NULL;
	eredis_reply_t *reply = NULL;
	/* event while */
	while(_w){
		/* stop while */
		if(_t % 10 == 0) _w = 0;
		data = open_file(SPATH_DATA);
		if(data == NULL){ printf("program error!! line:%d,function %s\n",__LINE__,__FUNCTION__); }
		root = cJSON_Parse(data);		
		free(data);
		if(root == NULL){ printf("program error!! line:%d,function %s\n message : %s\n",__LINE__,__FUNCTION__,cJSON_GetErrorPtr()); }
		size = cJSON_GetArraySize(root);
		if(size){
			for(i = 0; i < size; i++){
				item = cJSON_GetArrayItem(root,i);
				name = cJSON_GetObjectItem(item,"name");
				type = cJSON_GetObjectItem(item,"type");
				path = cJSON_GetObjectItem(item,"path");
				date = cJSON_GetObjectItem(item,"date");
				source = cJSON_GetObjectItem(item,"source");
				alternate = cJSON_GetObjectItem(item,"alternate");
				explain = cJSON_GetObjectItem(item,"explain");
				if(set_time % 10 == 0){
					printf("set!!\n");
					ert = init_engine();
					data = cJSON_Print(item);
					eredis_w_cmd(ert,"set %s %s",name->valuestring,data);
					free(data);
					r = eredis_r( ert );printf("line:%d;\n",__LINE__);
					eredis_r_append_cmd( r, "GET *group_name");
					reply = eredis_r_reply( r );
					if(reply->type == 1 || reply->type != 4 && reply->type != 6){
						group = cJSON_Parse(reply->str);
						if(group == NULL){ printf("program error!! line:%d,function %s\n message : %s\n",__LINE__,__FUNCTION__,cJSON_GetErrorPtr()); }
						cJSON_AddItemToArray( group, cJSON_CreateString(name->valuestring));
						data = cJSON_Print(group);
						eredis_w_cmd(ert,"set *group_name %s",data);
						free(data);
						cJSON_Delete(group);
					}else{
						eredis_w_cmd(ert,"set *group_name [\"%s\"]",name->valuestring);
					}
				}
#ifdef _DEBUG
	printf("name: %s;\n",name->valuestring);
	printf("type: %s;\n",type->valuestring);
	printf("path: %s;\n",path->valuestring);
	printf("date: %s;\n",date->valuestring);
	printf("source: %s;\n",source->valuestring);
	printf("alternate: %s;\n",alternate->valuestring);
	printf("explain: %s;\n%s\n\n",explain->valuestring,cJSON_Print(item));
#endif
			}
			cJSON_Delete(root);
		}
		usleep(_t);
		set_time += t/1000000;
	}
}


#undef SNAME
#undef STYPE
#undef SPATH
#undef SDATE
#undef SARRAY
#undef SALIAS
#undef SEXPLAIN
#undef SPATH_DATA
