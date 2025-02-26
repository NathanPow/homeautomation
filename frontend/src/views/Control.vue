<template>
    <v-container class="container" color="surface" justify = 'center' align-center flat>
        <v-row class="row" justify = 'center'>
            <v-col class="col" align-center>
                    <v-card class="text-secondary" title="Combination" color="surface" subtitle="Enter your four digit passcode" variant="tonal" align="center" flat>
                        <v-card-text>
                            <div class="passcode-container">
                                <v-otp-input length="4" v-model="passcode" class="passcode-input"></v-otp-input>
                            </div>
                        </v-card-text>
                    </v-card><br>
              
              <v-btn class="submit" @click="readPasscode();" color="primary">Submit</v-btn>
          </v-col>
      </v-row>

  </v-container>
</template>

<script setup>
/** JAVASCRIPT HERE */

// IMPORTS
import { ref,reactive,watch ,onMounted,onBeforeUnmount,computed } from "vue";  
import { useRoute ,useRouter } from "vue-router";
import { useMqttStore } from "@/store/mqttStore";
import { useAppStore } from "@/store/appStore";
import { storeToRefs } from "pinia";

// VARIABLES
const Mqtt = useMqttStore();
const AppStore = useAppStore();
const { payload, payloadTopic } = storeToRefs(Mqtt);
 
// VARIABLES
const router      = useRouter();
const route       = useRoute();  
const passcode = ref([""]);

// FUNCTIONS
// const submit = async () => {
//   const foo = await AppStore.setPasscode(passcode.value);
// };
function readPasscode() {
    // Code to read passcode here
    console.log(passcode.value);
    AppStore.setPasscode(passcode.value);
    
}

onMounted(()=>{
    // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
    Mqtt.connect();
    setTimeout( ()=>{ 
        // Subscribe to each topic 
        Mqtt.subscribe("620163595"); 
        Mqtt.subscribe("620163595_sub"); 
    },3000);
});

onBeforeUnmount(()=>{
    // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
    Mqtt.unsubcribeAll();
});
</script>

<style scoped>
/** CSS STYLE HERE */
</style>