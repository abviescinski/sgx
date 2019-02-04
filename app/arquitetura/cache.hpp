
class cache_t{
    public:
        cache_t();
        virtual ~cache_t();


        void allocate(uint32_t nivel);
        void estatisticas();
        
        //Calculos de reconhecimento de endereços
        uint32_t calcula_idx(uint64_t address);
        
        //Funções que busca endereços
        uint32_t busca_end(uint64_t address);
        inline uint32_t busca_LRU(conj_cache_t *conjunto);

		//Funções de escritas de entradas
        uint32_t writeAllocate(uint64_t address);
        inline void writeBack(uint64_t address);
        uint32_t add_entrada(uint64_t address);
        
        //mover entrada entre as caches
        void move_ent(uint64_t address,cache_t *cache,entrada_c *linha);
        void retorna_ent(uint64_t address,cache_t *cache);
        
        //atributtes
        uint32_t id;
        uint32_t nivel;
        uint32_t id_conjunto;
        uint32_t vias;
        conj_cache_t *conjunto;
        uint32_t shiftData;
        uint32_t cacheHit;
        uint32_t cacheMiss;
        uint32_t cacheAccess;

};
