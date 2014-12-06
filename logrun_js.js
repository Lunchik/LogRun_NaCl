/**
 * @file logrun_js.js
 * @author garipova.nailya@tgtoil.com
 * @author Copyright(C) 2012 TGT Oil & Gas Services. All right reserved.
 */
var BLOCK_SIZE = 50;
var received_base64 = window.webkitIntent.data;
//var blocks = received_base64/BLOCK_SIZE|0;
//var ind=0;
var messageEvent;
var once = 0;
var baseMine = base64_bin;
var binary_data = baseMine.decode(received_base64);
var arr1 = new Uint8Array(binary_data);
var string1 = new Array();



document.body.onload = pageDidLoad();

var FileIO1Module = null;

function moduleDidLoad() {
    FileIO1Module = document.getElementById("base64");
    console.log('SUCCESS');
    if (!once) {
        for (var ii=0; ii<arr1.length; ii++)
        {
            //string1 += " ";
            string1 += String.fromCharCode(arr1[ii]);
        }
        once = 1;
    }
    //var string2 = "lalila";
    //var string1 = new Array(arr1);
    FileIO1Module.postMessage(received_base64/*.substring(0,1000)*/);
    //FileIO1Module.postMessage("aaa\0nnn");
}

function handleMessage(message_event) {
    messageEvent = message_event;
    console.log(message_event);
    //document.write(message_event.data + "\n");
    /*if (ind<blocks-1){
        ind++;
        FileIO1Module.postMessage(ind);
    }      */

    //console.log(message_event.data);
}

function pageDidLoad() {

    if (typeof FileIO1Module == 'undefined')
        FileIO1Module = null;
    if (FileIO1Module == null) {
        console.log('LOADING...');
    } else {
        // It's possible that the Native Client module onload event fired
        // before the page's onload event.  In this case, the status message
        // will reflect 'SUCCESS', but won't be displayed.  This call will
        // display the current message.
        console.log('FileIO1Module was found before body onload');
    }
}