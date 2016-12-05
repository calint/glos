//------------------------------------------------------------------------ dync
#pragma once

#define dync_initial_capacity 8
#define dync_bounds_check 1

//------------------------------------------------------------------------- def

typedef struct dync{

	char *data;

	unsigned size;

	unsigned capacity;

}dync;

dync dync_def={0,0,0};

//--------------------------------------------------------------------- private

inline static void _dync_insure_free_capcity(dync*this,size_t n){
	const unsigned rem=this->capacity-this->size;
	if(rem>=n)
		return;
	if(this->data){
		unsigned new_cap=this->capacity*2;
		char *new_data=realloc(this->data,sizeof(char)*new_cap);
		if(!new_data){
			fprintf(stderr,"\nout-of-memory");
			fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
			exit(-1);
		}
		if(new_data!=this->data){
			this->data=new_data;
		}
		this->capacity=new_cap;
		return;
	}
	this->capacity=dync_initial_capacity;
	this->data=malloc(sizeof(char)*this->capacity);
	if(!this->data){
		fprintf(stderr,"\nout-of-memory");
		fprintf(stderr,"\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
}

//---------------------------------------------------------------------- public

inline static void dync_add(dync*this,char o){
	_dync_insure_free_capcity(this,1);
	*(this->data+this->size++)=o;
}

//-----------------------------------------------------------------------------

inline static char dync_get(dync*this,size_t index){
#ifdef dync_bounds_check
	if(index>=this->capacity){
		fprintf(stderr,"\nindex-out-of-bounds");
		fprintf(stderr,"\t%s\n\n%d  index: %zu    capacity: %zu\n",
				__FILE__,__LINE__,index,this->capacity);
		exit(-1);
	}
#endif
	char p=*(this->data+index);
	return p;
}

//-----------------------------------------------------------------------------

inline static char dync_get_last(dync*this){
	char p=*(this->data+this->size-1);
	return p;
}

//-----------------------------------------------------------------------------

inline static size_t dync_size_in_bytes(dync*this){
	return this->size*sizeof(char);
}

//-----------------------------------------------------------------------------

inline static void dync_free(dync*this){
	if(!this->data)
		return;
	free(this->data);
}

//-----------------------------------------------------------------------------

inline static void dync_add_list(dync*this,/*copies*/const char*str,int n){
	//? optimize
	const char*p=str;
	while(n--){
		_dync_insure_free_capcity(this,1);
		*(this->data+this->size++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dync_add_string(dync*this,/*copies*/const char*str){
	//? optimize
	const char*p=str;
	while(*p){
		_dync_insure_free_capcity(this,1);
		*(this->data+this->size++)=*p++;
	}
}

//-----------------------------------------------------------------------------

inline static void dync_write_to_fd(dync*this,int fd){
	if(!this->data)
		return;
	write(fd,this->data,this->size);
}

//-----------------------------------------------------------------------------

