function GetQueryString(name) {
    var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)");
    var r = window.location.search.substr(1).match(reg);
    // console.log(r) // for debug
    if (r != null)
        return decodeURI(r[2]);
    return null;
  }