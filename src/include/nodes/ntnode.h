
// ntnode 

/*
    ntnode [scaling.ip]
        -- new()
        -- dstr()
        -- up(tid,)
            
        -- run()

*/

/*
    ntnodedriver
    
        -- metrics
        -- status
        -- node*

        -- tid get_process();

        -- ctor()
        -- dstr()

        -- describend(ntuid, flags?)
        -- describe()

        -- rename(ntuid, newname)
        -- rescale_node(ntuid, new scale.node)
        -- rescale_port(ntuid, new scale.port)
        -- rescale(scale.config)

        -- get(ntuid)
            nodes[ntuid]
        -- new(config)
            new node(config)
            nodes.add(node, run|wait)
        -- del(ntuid)
            kill()
            dstr()

        -- status(ntuid)
        
        // FEATURES:
        - flow with balance/rebalance?

        -- start(ntuid)
        -- stop(ntuid)
        -- restart(ntuid)
        -- kill(ntuid)
        
        -- config(ntuid)
        -- config_modify(ntuid, newconfig)


*/

// 

