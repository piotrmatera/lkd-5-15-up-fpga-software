/*
 * stats.h
 *
 *  Created on: 4 lis 2023
 *      Author: Piotr
 */

#ifndef SOFTWARE_STATS_H_
#define SOFTWARE_STATS_H_


class stats_t{
public:

    typedef enum {
        nv_no_valid_copy,

        stats_max

    } stats_types;

private:
    Uint16 counters[stats_max];

public:
    stats_t(void){
        clear_all();
    }

    void clear_all(){
        memset( counters, 0, sizeof(counters));
    }

    void clear( stats_types stat ){
        if( stat < stats_max )
            counters[ stat ] = 0;
    }

    void increment( stats_types stat ){
        if( stat < stats_max )
            counters[ stat ]++;
    }

    Uint16 get_counter( stats_types stat ) const{
        if( stat < stats_max )
            return counters[ stat ];
        return 0;
    }


};

extern stats_t stats;


#endif /* SOFTWARE_STATS_H_ */
