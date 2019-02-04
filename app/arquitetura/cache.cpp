#include "simulator.hpp"

cache_t::cache_t()
{
    //ctor
}

cache_t::~cache_t()
{
    //dtor
}

//Função para alocar as caches, utilizada por simulator.cpp
void cache_t::allocate(uint32_t nivel){
	
	switch(nivel){
		case L1:{
			this->shiftData = 6;
			this->nivel = nivel;
			this->cacheHit=0;
			this->cacheMiss=0;
			this->cacheAccess=0;
			
			//alocando a cache L1
			this->id_conjunto = L1_CONJ;
			this->vias = L1_ASSOC;
			this->conjunto = new conj_cache_t[L1_CONJ];
			
			for (size_t i = 0; i < L1_CONJ; i++){
				this->conjunto[i].entradas = new entrada_c[L1_ASSOC];
				memset(this->conjunto[i].entradas,0,B_LINHA);
			}
			break;
			}
		case L2:{
			this->shiftData = 6;
			this->nivel = nivel;
			this->cacheHit=0;
			this->cacheMiss=0;
			this->cacheAccess=0;
			this->id_conjunto = L2_CONJ;
			this->vias = L2_ASSOC;
			this->conjunto = new conj_cache_t[L2_CONJ];
			for (size_t i = 0; i < L2_CONJ; i++){
				this->conjunto[i].entradas = new entrada_c[L2_ASSOC];
				memset(this->conjunto[i].entradas,0,B_LINHA);
			}
			break;
			}
		}
};

//Função  para calcular o ID do conjunto
uint32_t cache_t::calcula_idx(uint64_t address){
	uint32_t getBits = (this->id_conjunto)-1;
	uint32_t tag = (address >> this->shiftData);
	uint32_t index = tag&getBits;	
	return index;
};

/*Busca o endereço em determinado conjunto, caso ele tenha sido encontrado (tag) retorna HIT e 
atualiza o ciclo de utilização, caso não so retorna MISS*/
uint32_t cache_t::busca_end(uint64_t address){
	uint32_t idx = this->calcula_idx(address);
	uint32_t tag = (address >> this->shiftData);
	for (size_t i = 0; i < this->vias; i++){
		if(this->conjunto[idx].entradas[i].tag == tag){
			this->conjunto[idx].entradas[i].LRU = orcs_engine.get_global_cycle();
			return HIT;
		}
	}
	return MISS;
};
/*adiciona uma nova entrada na cache (for), se nenhuma for encontrada para ser substituida busca a mais antiga, 
realiza writeback e aloca a nova entrada*/  
uint32_t cache_t::add_entrada(uint64_t address){
	uint32_t idx = this->calcula_idx(address);
	uint32_t tag = (address >> this->shiftData);
	for (size_t i = 0; i < this->vias; i++){
		if(this->conjunto[idx].entradas[i].validade==0){
			this->conjunto[idx].entradas[i].tag =tag;
			this->conjunto[idx].entradas[i].LRU = orcs_engine.get_global_cycle();
			this->conjunto[idx].entradas[i].validade = 1;
			this->conjunto[idx].entradas[i].usado = 0;
			return OK;
		}
	}
	uint32_t line = this->busca_LRU(&this->conjunto[idx]);
	if(this->conjunto[idx].entradas[line].usado==1){
		this->writeBack(address);
	}
	this->conjunto[idx].entradas[line].tag = tag;
	this->conjunto[idx].entradas[line].LRU = orcs_engine.get_global_cycle();
	this->conjunto[idx].entradas[line].validade = 1;	
	this->conjunto[idx].entradas[line].usado = 0;	
	return OK;
};
//Busca a entrada mais antiga do conjunto(coluna)
inline uint32_t cache_t::busca_LRU(conj_cache_t *conjunto){
	uint32_t index=0;
	for (size_t i = 1; i < this->vias; i++){
		index = (conjunto->entradas[index].LRU <= conjunto->entradas[i].LRU)? index : i ;
	}
	return index;
};
//Move a entrada da L1 para L2 se não restaura endereço da L1
inline void cache_t::writeBack(uint64_t address){
	uint32_t idx = this->calcula_idx(address);
	uint32_t line = this->busca_LRU(&this->conjunto[idx]);
	if(this->nivel == L1){
		//move entrada para L2
		this->move_ent(address,&orcs_engine.cache[L2],&this->conjunto[idx].entradas[line]);
		orcs_engine.global_cycle+=L2_LATENCIA;
	}else{
		memset(&this->conjunto[idx].entradas[line],0,sizeof(entrada_c));
		orcs_engine.global_cycle+=RAM_LATENCIA;
	}
};
//Se caso a entrada for encontrada na L2, move para a L1
void cache_t::retorna_ent(uint64_t address,cache_t *cache){
	uint32_t idx = this->calcula_idx(address);
	uint32_t line=0;
	uint32_t tag = (address >> this->shiftData);
	for (size_t i = 0; i < this->vias; i++){
		if(this->conjunto[idx].entradas[i].tag==tag){
			line = i;
		}
	}	
	this->move_ent(address,cache,&this->conjunto[idx].entradas[line]);
};
//Caso entrada tiver sido usada realiza WriteBack, restaura e faz novas atribuições
void cache_t::move_ent(uint64_t address,cache_t *cache, entrada_c *linha){
	uint32_t idx = cache->calcula_idx(address);
	uint32_t line = cache->busca_LRU(&cache->conjunto[idx]);
	uint32_t tag = (address >> cache->shiftData);
	if(cache->conjunto[idx].entradas[line].usado==1){
		cache->writeBack(address);
	}
	memcpy(&cache->conjunto[idx].entradas[line],linha,sizeof(entrada_c));
	cache->conjunto[idx].entradas[line].tag = tag;
	cache->conjunto[idx].entradas[line].LRU = orcs_engine.get_global_cycle();
};
/*Busca endereço no conjunto L1, caso tenha encontrado confirma pela tag, atribui o uso e atualiza o ciclo,
caso não confirmou a tag adiciona nova entrada, caso tenha sido MISS busca na L2*/
uint32_t cache_t::writeAllocate(uint64_t address){
	uint32_t resp = this->busca_end(address);
	uint32_t idx = this->calcula_idx(address);
	uint32_t tag = (address >> this->shiftData);
	if(resp == HIT){
		this->cacheHit++;
		for (size_t i = 0; i < this->vias; i++){
			if(this->conjunto[idx].entradas[i].tag==tag){
				this->conjunto[idx].entradas[i].usado=1;
				this->conjunto[idx].entradas[i].LRU=orcs_engine.get_global_cycle();
			}
		}
		this->add_entrada(address);
		for (size_t i = 0; i < this->vias; i++){
			if(this->conjunto[idx].entradas[i].tag==tag){
				this->conjunto[idx].entradas[i].usado=1;
				this->conjunto[idx].entradas[i].LRU=orcs_engine.get_global_cycle();
				return OK;
			}
		}
	}else{
		this->cacheMiss++;
		//consulta L2 se ta la traz pra L1 senao instala nova
		if(orcs_engine.cache[L2].busca_end(address)==HIT){
			this->retorna_ent(address,this);
		}else{
			this->add_entrada(address);
		}
		
		for (size_t i = 0; i < this->vias; i++){
			if(this->conjunto[idx].entradas[i].tag==tag){
				this->conjunto[idx].entradas[i].usado=1;
				this->conjunto[idx].entradas[i].LRU=orcs_engine.get_global_cycle();
				return OK;
			}
		}
	}
	return OK;
};

//apresenta as estatisticas de cada cache, chamada pelo simulator
void cache_t::estatisticas(){
	ORCS_PRINTF("Cache Level: %i\n",this->nivel+1);
	ORCS_PRINTF("Cache Access: %i\n",this->cacheAccess);
	ORCS_PRINTF("Cache Hits: %i\n",this->cacheHit);
	ORCS_PRINTF("Cache Miss: %i\n",this->cacheMiss);
	
	ORCS_PRINTF("Cache Hits: %f\n",((float(this->cacheHit))*100/this->cacheAccess));
	ORCS_PRINTF("Cache Miss: %f\n",((float(this->cacheMiss))*100/this->cacheAccess));

}
