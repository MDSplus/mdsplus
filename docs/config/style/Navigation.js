

// load xml file
function addNavPathLi(ul) {
    if (window.XMLHttpRequest) {
        xhttp = new XMLHttpRequest();
    } else {    // IE 5/6
        xhttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
    
    xhttp.overrideMimeType('text/xml');
    xhttp.open("GET", "nav.xml", true);
    xhttp.send();
    xhttp.onreadystatechange = function () {
        if ( xhttp.readyState == 4 && xhttp.status==200 ) {                      
            var dir = xhttp.responseXML.getElementsByTagName("topdir")[0].childNodes[0].nodeValue;          
            var prj = xhttp.responseXML.getElementsByTagName("dxproject");
            for (var i = 0; i < prj.length; i++) {
                y = prj[i];
                var name = y.getAttribute("name");
                var url  = y.getAttribute("url");
                
                // var ul = document.getElementById("nav-list");
                var li = document.createElement("li");        
                li.setAttribute("class", "navelem");
                var a = document.createElement("a");
                a.setAttribute("class", "el");
                a.setAttribute("href", dir+url+"/index.html");
                var span = document.createElement("span");
                  span.appendChild(document.createTextNode( name ));
                a.appendChild(span);
                li.appendChild(a);
                ul.appendChild(li);                        
                
                y = y.nextSibling;
            }    
        }
    };    
}



function addNavPathUl(div) {
    // var nav_div = document.getElementById(div);
    var ul = document.createElement("ul");
    ul.setAttribute("id", "nav-list");
    ul.setAttribute("class", "navlist");
    ul.setAttribute("style", "float: left; margin-top: 0px; margin-bottom: 0px;");
    
    addNavPathLi(ul)
    if(div.childNodes[0].length > 0)
        div.insertBefore(ul,div.childNodes[0]);
    else
        div.appendChild(ul);
    return ul;
}







function hideNavTree() {    
    $("#navrow2").css({display: 'block'});
    $("#navrow3").css({display: 'block'});
    $("#navrow4").css({display: 'block'});
    $("#navrow5").css({display: 'block'});
    writeCookie("width",0,null);
    restoreWidth(0);
    resizeHeight();
}

function showNavTree() {
    $("#navrow2").css({display: 'none'});
    $("#navrow3").css({display: 'none'});
    $("#navrow4").css({display: 'none'});
    $("#navrow5").css({display: 'none'});
    var width = readCookie("width");
    if(width == 0) { 
        width = 250; 
        writeCookie("width",width,null);
    }
    restoreWidth(width);
    resizeHeight();
}


// add toggle button //
function addToggleButton(div) {
    var navrow = div;
    var toggle_button = document.createElement("div");
    var toggle_button_img = document.createElement("img");
    
    toggle_button.setAttribute("id","nav-toggle");
    toggle_button.setAttribute("style","float: left;");
    toggle_button_img.setAttribute("title","click to disable nav panel");
    toggle_button_img.setAttribute("src","sync_on.png");
    toggle_button.appendChild(toggle_button_img);
    
    navrow.insertBefore(toggle_button, navrow.childNodes[0]);
        
    
    // var button = document.getElementById('nav-toggle');
    toggle_button.onclick = function() {
        var nav = document.getElementById('side-nav');
        var dct = document.getElementById('doc-content');        
        
        var toggle = readCookie("toggle");
        
        if (toggle !== 'none') {
            toggle = 'none';
            hideNavTree();
        }
        else {
            toggle = 'block';
            showNavTree();
        }            
        writeCookie("toggle",toggle,null);        
    };
    return toggle_button;
}
