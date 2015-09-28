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


//<ul id="nav-list">    
//  <div margin-top="5px">$searchbox</div>
//  </ul>  
//</div>


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
        var width = 200;
        if (nav.style.display !== 'none') {
            nav.style.display = 'none';
            width = nav.style.width.value; // fix this ...
            nav.style.width = '0px';
            dct.style.marginLeft = '0px';
        }
        else {
            nav.style.display = 'block';        
            nav.style.width = width + 'px';
            dct.style.marginLeft = width + 'px';
        }
    };
    return toggle_button;
}
