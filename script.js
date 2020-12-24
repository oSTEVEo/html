var DEBUG = false;

var key = 0;

function httpGet(theUrl, callback) {
  console.log(theUrl);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", theUrl, true);

  xhr.onreadystatechange = function () {
    //Call a function when the state changes.
    if (xhr.readyState == 4 && xhr.status == 200) {
      callback(xhr.responseText);
    }
  };
  xhr.send(null);
}

function get_temp() {
  //console.log("get temp");
  var value;

  httpGet("/get_temp", function (value) {
    console.log(value);
    var tmp = JSON.parse(value);

    document.getElementById("temp0").innerText = `0 >\ ${tmp.t0.set+"/"+tmp.t0.current} C°`;
    document.getElementById("temp1").innerText = `1 >\ ${tmp.t1.set+"/"+tmp.t1.current} C°`;
    document.getElementById("temp2").innerText = `2 >\ ${tmp.t2.set+"/"+tmp.t2.current} C°`;
    document.getElementById("temp3").innerText = `3 >\ ${tmp.t3.set+"/"+tmp.t3.current} C°`;
  });
}

function get_fq() {
  //console.log("get fq");
  if (DEBUG) value = '{"f0":"0","f1":"0"}';
  else
    value = httpGet("/get_fq", function (value) {
      console.log(value);
      var tmp = JSON.parse(value);

      document.getElementById("fq0").innerText = `Мотор 0 > ${tmp.fq0} Hz`;
      document.getElementById("fq1").innerText = `Мотор 1 > ${tmp.fq1} Hz`;
    });

  //console.log(value);
}

function get_PID() {
  /*
    <td>Нагреватель 1</td>
    <td id="pid0_kp">-1</td>
    <td id="pid0_ki">-1</td>
    <td id="pid0_kd">-1</td>
    */
  httpGet("/get_PID", function (value) {
    console.log(value);

    var tmp = JSON.parse(value);

    var a = Object.values(tmp);
    for (var i = 0; i < 4; i++) {
      document.getElementById(`pid${i}_kp`).innerText = a[i].Kp;
      document.getElementById(`pid${i}_ki`).innerText = a[i].Ki;
      document.getElementById(`pid${i}_kd`).innerText = a[i].Kd;
    }
  });
}

function set_temp() {
  //chen_num, value
  var chen_num = document.getElementById("temps_form").value;
  var value = document.getElementById("set_temp").value;
  console.log("set temp");
  var request = "/set_temp?chenl=";
  request += chen_num;
  request += "&temp=";
  request += value;
  request += "&key=";
  request += key;

  httpGet(request, function (text) {
    console.log("set_temp reqest-" + text);
  });
}

function set_fq() {
  var chen_num = document.getElementById("fq_form").value;
  var frequency = document.getElementById("set_fq").value;
  console.log("set fq");
  var request = "/set_fq?chenl=";
  request += chen_num;
  request += "&frequency=";
  request += frequency;
  request += "&key=";
  request += key;

  httpGet(request, function (text) {
    console.log("set_fq reqest -" + text);
  });
}

function set_PID() {
  var chenl = document.getElementById("temps_for_pid").value;

  console.log("set PID");
  var request = "/set_PID?chenl=";
  request += chenl;
  request += "&kn=";
  request += document.getElementById("pid_form").value;
  request += "&value=";
  request += document.getElementById("set_pid").value;
  request += "&key=";
  request += key;

  httpGet(request, function (text) {
    console.log("set_pid reqest -" + text);
  });
}

setInterval(function () {
  get_temp();
  get_fq();
  get_PID();
}, 1500);
//setInterval(get_fq, 1500);
//setInterval(get_PID, 1500);

//get_temp();
//get_fq();
//get_PID();

//set_temp(1, 2);
//set_fq(1, 2);
//set_PID(1, 2, 3, 4);
//Сделай временные переменные для пида чтобы закидывать их в запрос getPID
