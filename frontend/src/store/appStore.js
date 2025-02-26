import {defineStore} from 'pinia'
import {ref} from 'vue'

export const useAppStore =  defineStore('app', ()=>{

    // STATES 

    // ACTIONS
        const getUpdateData= async (data) => {
            const controller = new AbortController();
            const signal = controller.signal;
            const id= setTimeout(() => controller.abort(), 60000);
            const URL= '/api/update';
            try {
                    const response = await fetch(URL, {
                        method: 'POST',
                        signal: signal,
                        headers: {
                            'Content-Type': 'application/json'
                        },
                        body: JSON.stringify(data)
                    });
                    if (!response.ok) {
                        console.error(`getUpdateData failed: ${response.status} ${response.statusText}`);
                        return {status: 'error', message: 'Request failed'};
                    }
                    const result = await response.json();
                    clearTimeout(id);
                    return result;
                } catch (error) {
                    clearTimeout(id);
                    console.error("getUpdateData error:", error.message);
                    return {status: 'getUpdatedata error', message: error.message};
                }
        }

        const setPasscode = async (data) => {
            const controller = new AbortController();
            const signal = controller.signal;
            const id = setTimeout(() => { controller.abort() }, 60000);
            const URL = `/api/set/combination`;
            console.log(data);
            let text = data.toString();
            try {
                const response = await fetch(URL, { method: 'POST', signal: signal, body: JSON.stringify({"code": text}), headers: {'Content-Type': 'application/json'}});
                if (!response.ok) {
                    console.error(`setPasscode failed: ${response.status} ${response.statusText}`);
                    return {status: 'error', message: 'Request failed'};
                }
                const result = await response.json();
                clearTimeout(id);
                return result;
            }
            catch (err) {
                clearTimeout(id);
                console.error("setPasscode error:", err.message);
                return {status: 'setPasscode error', message: err.message};
            }
        }

        const getCheckPwd= async (data) => {
            const controller = new AbortController();
            const signal = controller.signal;
            const id= setTimeout(() => controller.abort(), 60000);
            const URL= '/api/check/combination'; 
            try {
                const response = await fetch(URL, {
                    method: 'POST',
                    signal: signal,
                    headers: {
                        'Content-Type': 'application/json'
                    },
                    body: JSON.stringify(data)
                });
                if (!response.ok) {
                    console.error(`getCheckPwd failed: ${response.status} ${response.statusText}`);
                    return {status: 'error', message: 'Request failed'};
                }
                const result = await response.json();
                clearTimeout(id);
                return result;
            } catch (error) {
                clearTimeout(id);
                console.error("getCheckPwd error:", error.message);
                return {status: 'getCheckPwd error', message: error.message};
            }
        }
                
        const getReserve = async (start, end) => {
            const controller = new AbortController();
            const signal = controller.signal;
            const id = setTimeout(() => { controller.abort() }, 60000);
            const URL = `/api/reserve/${start}/${end}`;
            try {
                console.log(`Fetching reserve data for ${start} to ${end}`);
                const response = await fetch(URL, { method: 'GET', signal: signal });
                if (!response.ok) {
                    console.error(`getReserve failed: ${response.status} ${response.statusText}`);
                    return [];
                }
                const data = await response.json();
                if (data && data.status === "found" && data.data) {
                    return data.data;
                }
                console.warn("getReserve returned no data:", data);
            }
            catch (err) {
                console.error('getReserve error:', err.message);
            }
            return [];
        }

        const updateCard = async (start,end) => {
            const controller = new AbortController();
            const signal = controller.signal;
            const id = setTimeout(() => { controller.abort() }, 60000);
            const URL = `/api/avg/${start}/${end}`;
            try {
                console.log(`Fetching updateCard data for ${start} to ${end}`);
                const response = await fetch(URL, { method: 'GET', signal: signal });
                if (!response.ok) {
                    console.error(`updateCard failed: ${response.status} ${response.statusText}`);
                    return [];
                }
                const data = await response.json();
                if (data && data.status === "found" && data.data) {
                    console.log("Data received:", data.data);
                    return data.data;
                }
                console.warn("updateCard returned no data:", data);
            }
            catch (err) {
                console.error('updateCard error:', err.message);
            }
            return [];
        }
    
    return { 
    // EXPORTS	
        getUpdateData,
        setPasscode,
        getCheckPwd,
        updateCard,
        getReserve
       }
},{ persist: true  });
