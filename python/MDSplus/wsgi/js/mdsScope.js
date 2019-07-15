var mouseTarget;
var ZOOM = 1;
var CROSSHAIR = 2;
var PAN = 3;
var mode = ZOOM;
//Double click management
var alreadyClicked = false;
var alreadyDoubleClicked = false;

var stopRightMouse=false;
var colorPalette;
var colorTables;
var contextMenuSvg; //svg above which righ button has been clicked
var globalShotPanels = new Array(); //Array containing information required for wave panels taking global shot number
var globalShots = new Array(); //Array containing global shot numbers (as defined in the bottom form)
var wavePanels = new Array();

//Limit costants used for  limits management
var LIMITS_XMIN = 1;
var LIMITS_XMAX = 2;
var LIMITS_YMIN = 3;
var LIMITS_YMAX = 4;

var METRICS_BORDER = 2;
var mouseMovingPanel; //The WavePanel into which the mouse is being moved

var dateTimeFormatHMS = new Intl.DateTimeFormat(undefined, {hour: 'numeric', minute: 'numeric', second: 'numeric', hour12: false});
var dateTimeFormatYMD = new Intl.DateTimeFormat(undefined, {year: 'numeric', month: 'short', day: 'numeric'});

var bottomLine = 
    '<form name="scopeMode" >'+
    '<table cellpadding="20" cellspacing="0" ><tr><td><label for="Zoom"><input type="radio" checked = "yes" name="Mode" id="Zoom" onclick="setMode(ZOOM);"/>Zoom</label>'+
    '<label for="Crosshair"> <input type="radio" name="Mode" id="Crosshair" onclick="setMode(CROSSHAIR);"/>Crosshair</label>'+
    '<label for="Pan"><input type="radio" name="Mode" id="Pan" onclick="setMode(PAN);"/> Pan</label></td><td>'+
    '<label for="GlobalShots">Shots</label> <input type = "text" id="GlobalShots"  onkeydown="if (event.keyCode == 13) {event.preventDefault(); updateGlobalShots();}"/>'+
    '<button type="button"  autofocus id = "UpdateGlobal" onclick="updateGlobalShots()">Update</button></td><td><label id="XValue" /></td><td><label id="YValue" /></td><td>'+
    '<label id="Expr" /></td><td align = "top"><label id="Shot" /></td></tr></table>'+
    '</form>';

////////////////Global mouse callback functions///////////////////////

function setMode(inMode)
{
    var idx;
    mode = inMode;
    for(idx = 0; idx < wavePanels.length; idx++)
    {
        wavePanels[idx].notifyMode(inMode);
    }
}

//var startZoomX, startZoomY;

function contextMenu(e)
{
    if (stopRightMouse) {
      stopRightMouse=false;
      e.stopPropagation();
      return false;
    } else
      return true;
}

function fixEvent(e)
{
    if (!e.hasOwnProperty('offsetX'))
    {
        // var curleft = curtop = 0;
        // if (e.offsetParent) {
        //   var obj=e;
        //   do {
        //     curleft += obj.offsetLeft;
        //     curtop += obj.offsetTop;
        //   } while (obj = obj.offsetParent);
        // } 
        // e.offsetX=e.layerX-curleft;
        // e.offsetY=e.layerY-curtop;
        // var bcr = e.currentTarget.parentNode.getBoundingClientRect();
        e.offsetX = e.layerX; //- bcr.left;
        e.offsetY = e.layerY; //- bcr.top;
    }
    return e;
}

function anyZooming()
{
    for(idx = 0; idx < wavePanels.length; idx++)
    {
        if(wavePanels[idx].isZooming())
            return true;
    }
    return false;
}
 
function mouseDown(e)
{
    var idx;
    if (e == null)
      e = window.event;
    e.preventDefault();
    e.stopPropagation();
    var targetType = e.currentTarget.parentNode.className;
    var popupMenu = document.getElementById(targetType + 'Popup');
    document.getElementById('WavePanelPopup').style.display = 'none';
    document.getElementById('FramePanelPopup').style.display = 'none';
    if(e.button == 2) //MB3 (right) button pressed
    {
        if(anyZooming())
            return false;
        stopRightMouse = true;
        contextMenuSvg = e.currentTarget;
        var targetPanel;
        for(idx = 0; idx < wavePanels.length; idx++)
        {
            if(wavePanels[idx].svg == e.currentTarget)
                targetPanel = wavePanels[idx];
        }
        if(targetType == 'WavePanel')
        {
            if(targetPanel.zoomCache.length == 0)
                document.getElementById('UndoZoomButton').disabled = true;
            else
                document.getElementById('UndoZoomButton').disabled = false;
        }
        else  // FramePanel
        {
            var playButton = document.getElementById('FramePlayButton');
            if(targetPanel.playing)
            {
                playButton.setAttribute('onclick', 'stopPlay()');
                playButton.textContent = 'Stop play';
            }
            else
            {
                playButton.setAttribute('onclick', 'startPlay()');
                playButton.textContent = 'Start play';
            }
        }
        var left = e.pageX - 120;
        var top = e.pageY - 70;
        if(left < 0)
          left = 0;
        if(top < 0)
          top = 0;
        popupMenu.style.left = left + 'px';
        popupMenu.style.top = top + 'px';
        popupMenu.style.display = 'inherit';
        e.stopPropagation();
        return false;
    }
    //check for double click
    if(!alreadyClicked)
    {
        alreadyClicked = true;
        setTimeout(function(){alreadyClicked = false;}, 300);
    }
    else //it is a double click
    {
        for(idx = 0; idx < wavePanels.length; idx++)
        {
            if(wavePanels[idx].svg == e.target)
            {
                switch(mode) {
                    case PAN:
                    case ZOOM: 
                        wavePanels[idx].resetScales();
                        break;
                }
            }
        }
        return;
    }
    mouseTarget = e.target;
    e = fixEvent(e);
    var downX, downY;
    if(e.target.localName == 'text')
    {
        downX = e.target.offsetLeft + e.offsetX;
        downY = e.target.offsetTop + e.offsetY;
    }
    else
    {
        downX = e.offsetX;
        downY = e.offsetY;
    }
    for(idx = 0; idx < wavePanels.length; idx++)
    {
        if(wavePanels[idx].svg == e.currentTarget)
        {
            mouseMovingPanel = wavePanels[idx];
            wavePanels[idx].enableMouseMove();
            switch(mode) {
                case ZOOM:                    
                    //startZoomX = downX;
                    //startZoomY = downY;
                    wavePanels[idx].startZoom(downX, downY);
                    break;
                case CROSSHAIR:
                    for(var currIdx = 0; currIdx < wavePanels.length; currIdx++)
                    {
                        if(currIdx != idx)
                            wavePanels[currIdx].endCrosshair();
                    }
                    wavePanels[idx].startCrosshair(downX, downY);
                    break;
                case PAN:
                    wavePanels[idx].startPan(downX, downY);
                    break;

            }
        }
    }
    return true;
}


function mouseMove(e)
{
    var idx;
    if ( e == null )
      e=window.event;
    e.preventDefault();
    fixEvent(e);
 //   if(mouseTarget != e.target)
 //       return;
    if(e.button == 2) //MB3 (right) button pressed
    {
        if(anyZooming())
            return;
    }
    if(mouseMovingPanel != undefined)
    {
        if(mouseMovingPanel.svg == e.currentTarget)
        {
            //e=fixEvent(e);
            var moveX, moveY;
            if(e.target.localName == 'text')
            {
                moveX = e.target.offsetLeft + e.offsetX;
                moveY = e.target.offsetTop + e.offsetY;
            }
            else
            {
                moveX = e.offsetX;
                moveY = e.offsetY;
            }
            switch(mode) {
                case ZOOM: 
                    // Used in case mouse button is released in a different target 
                    mouseMovingPanel.dragZoom(moveX, moveY);
                    break;
                case CROSSHAIR:
                    mouseMovingPanel.dragCrosshair(moveX, moveY);
                    break;
                case PAN:
                    mouseMovingPanel.dragPan(moveX, moveY);
                    break;
            }
        }
    }
}
function mouseUp(e)
{
    var idx;
    if (e == null)
      e=window.event;
 //   e.preventDefault();
    fixEvent(e);
    var upX, upY;
    if(e.target.localName == 'text')
    {
        upX = e.target.offsetLeft + e.offsetX;
        upY = e.target.offsetTop + e.offsetY;
    }
    else
    {
        upX = e.offsetX;
        upY = e.offsetY;
    }

    //Handle the possibility that the mouse button is released on another panel
    if(mode == ZOOM && mouseMovingPanel != undefined)
    {
        mouseMovingPanel.disableMouseMove();
        if(mouseMovingPanel.isZooming())
        {
            e=fixEvent(e);
            mouseMovingPanel.endZoom(upX, upY);
        }
    }
    else
    {
        for(idx = 0; idx < wavePanels.length; idx++)
        {
            e=fixEvent(e);
            switch(mode) {
                case CROSSHAIR:
                    wavePanels[idx].disableMouseMove();
                    wavePanels[idx].endCrosshair();
                    break;
                case PAN:
                    wavePanels[idx].disableMouseMove();
                    wavePanels[idx].endPan();
                    break;
                }
        }
    }
}

//Touch event management 
var touchMoved;
function touchStart(e)
{
    if (e == null)
      e=window.event;
    var idx;
    contextMenuSvg = e.target;
    e.preventDefault();
    touchMoved = false;
    var targetType = e.currentTarget.parentNode.className;
    var popupMenu = document.getElementById(targetType + 'Popup');
    if(!alreadyClicked)
    {
        alreadyClicked = true;
        setTimeout(function(){alreadyClicked = false;}, 300);
    }
    else //it is a double click
    {
        popupMenu.style.display = 'inherit';
        popupMenu.style.left = e.touches[0].clientX + 'px';
        popupMenu.style.top = e.touches[0].clientY + 'px';
        e.stopPropagation();
        return ;
    }
    document.getElementById('WavePanelPopup').style.display = 'none';
    document.getElementById('FramePanelPopup').style.display = 'none';
    e.preventDefault();
    //alert(''+svg.getElementsByTagName("rect")[0].x.value);
    for(idx = 0; idx < wavePanels.length; idx++)
    { 
        if(wavePanels[idx].svg == e.target)
        {
            var touchX = e.touches[0].clientX - wavePanels[idx].col*e.target.clientWidth;
            var touchY = e.touches[0].clientY - wavePanels[idx].row*e.target.clientHeight;
            switch(mode) {
                case ZOOM: 
                    wavePanels[idx].startZoom(touchX, touchY);
                    break;
                case CROSSHAIR:
                    wavePanels[idx].startCrosshair(touchX, touchY);
                    break;
                case PAN:
                    wavePanels[idx].startPan(touchX, touchY);
                    break;

            }
         }
    }
}

var touchMoveX, touchMoveY;
function touchMove(e)
{
    if (e == null) 
      e=window.event;
    var idx;
    e.preventDefault();
    touchMoved = true;
    var node = e.target;
    for(idx = 0; idx < wavePanels.length; idx++)
    {
        if(wavePanels[idx].svg == e.target)
        {
            touchMoveX = e.touches[0].clientX - wavePanels[idx].col*e.target.clientWidth;
            touchMoveY = e.touches[0].clientY- wavePanels[idx].row*e.target.clientHeight;
            switch(mode) {
                case ZOOM: 
                    wavePanels[idx].dragZoom(touchMoveX, touchMoveY);
                    break;
                case CROSSHAIR:
                    wavePanels[idx].dragCrosshair(touchMoveX, touchMoveY);
                    break;
                case PAN:
                    wavePanels[idx].dragPan(touchMoveX, touchMoveY);
                    break;
            }
        }
    }
}
function touchEnd(e)
{
    if (e == null)
      e=window.event;
    var idx;
    e.preventDefault();
    for(idx = 0; idx < wavePanels.length; idx++)
    {
        if(wavePanels[idx].svg == e.target)
        {
            switch(mode) {
                case ZOOM: 
                    if(!touchMoved) wavePanels[idx].clicked(touchMoveX, touchMoveY);
                    wavePanels[idx].endZoom(touchMoveX, touchMoveY);
                    break;
                case CROSSHAIR:
                    wavePanels[idx].endCrosshair();
                    break;
                case PAN:
                    wavePanels[idx].endPan();
                    break;
            }
        }
    }
}
///////////////////////////////////////////////////////////////////////////
/** 
 * Button callback functions and  functions dealing with multiple wave panels (contained in wavePanels array
 **/
function dragOtherCrosshairs(originatorPanel, x, y)
{
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        wavePanels[panelIdx].dragCrosshairExternal(x, y);
    }
}

function autoscale()
{
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
            wavePanels[panelIdx].autoscale();
    }
    document.getElementById('WavePanelPopup').style.display = 'none';
    document.getElementById('FramePanelPopup').style.display = 'none';
}
function autoscaleAll()
{
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        wavePanels[panelIdx].autoscale();
    }
    document.getElementById('WavePanelPopup').style.display = 'none';
    document.getElementById('FramePanelPopup').style.display = 'none';
}
function autoscaleAllImages()
{
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].canvas != undefined)
            wavePanels[panelIdx].autoscale();
    }
    document.getElementById('WavePanelPopup').style.display = 'none';
    document.getElementById('FramePanelPopup').style.display = 'none';
}

function autoscaleY()
{
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
            wavePanels[panelIdx].autoscaleY();
    }
    document.getElementById('WavePanelPopup').style.display = 'none';
    document.getElementById('FramePanelPopup').style.display = 'none';
}
function allSameScale()
{
    var xMin, xMax, yMin, yMax;
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
        {
            xMin = wavePanels[panelIdx].metrics.xmin;
            xMax = wavePanels[panelIdx].metrics.xmax;
            yMin = wavePanels[panelIdx].metrics.ymin;
            yMax = wavePanels[panelIdx].metrics.ymax;
            break;
        }
    }
    if(xMin == undefined) return;
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg != contextMenuSvg)
        {
            wavePanels[panelIdx].setScale(xMin, xMax, yMin, yMax);
        }
    }
    document.getElementById('WavePanelPopup').style.display = 'none';
    document.getElementById('FramePanelPopup').style.display = 'none';
}
function allSameXScale()
{
    var xMin, xMax;
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
        {
            xMin = wavePanels[panelIdx].metrics.xmin;
            xMax = wavePanels[panelIdx].metrics.xmax;
            break;
        }
    }
    if(xMin == undefined) return;
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg != contextMenuSvg)
        {
            wavePanels[panelIdx].setScale(xMin, xMax, wavePanels[panelIdx].metrics.ymin, 
                wavePanels[panelIdx].metrics.ymax);
        }
    }
    document.getElementById('WavePanelPopup').style.display = 'none';
    document.getElementById('FramePanelPopup').style.display = 'none';
}
function allSameXScaleAutoY()
{
    var xMin, xMax;
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
        {
            xMin = wavePanels[panelIdx].metrics.xmin;
            xMax = wavePanels[panelIdx].metrics.xmax;
            break;
        }
    }
    if(xMin == undefined) return;
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg != contextMenuSvg)
        {
            wavePanels[panelIdx].setScaleAutoY(xMin, xMax);
        }
    }
    document.getElementById('WavePanelPopup').style.display = 'none';
    document.getElementById('FramePanelPopup').style.display = 'none';
}
function allSameYScale()
{
    var yMin, yMax;
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
        {
            yMin = wavePanels[panelIdx].metrics.ymin;
            yMax = wavePanels[panelIdx].metrics.ymax;
            break;
        }
    }
    if(yMin == undefined) return;
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg != contextMenuSvg)
        {
            wavePanels[panelIdx].setScale(wavePanels[panelIdx].metrics.xmin, wavePanels[panelIdx].metrics.xmax, 
                yMin, yMax);
        }
    }
    document.getElementById('WavePanelPopup').style.display = 'none';
    document.getElementById('FramePanelPopup').style.display = 'none';
}

function resetAllScales()
{
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        wavePanels[panelIdx].resetScales();
    }
    document.getElementById('WavePanelPopup').style.display = 'none';
    document.getElementById('FramePanelPopup').style.display = 'none';
}

function resetScales()
{
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
            wavePanels[panelIdx].resetScales();
    }
    document.getElementById('WavePanelPopup').style.display = 'none';
    document.getElementById('FramePanelPopup').style.display = 'none';
}

function undoZoom()
{
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
            wavePanels[panelIdx].undoZoom();
    }
    document.getElementById('WavePanelPopup').style.display = 'none';
    document.getElementById('FramePanelPopup').style.display = 'none';
}

function startPlay()
{
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
            wavePanels[panelIdx].startPlay();
    }
    document.getElementById('WavePanelPopup').style.display = 'none';
    document.getElementById('FramePanelPopup').style.display = 'none';
}

function stopPlay()
{
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
            wavePanels[panelIdx].stopPlay();
    }
    document.getElementById('WavePanelPopup').style.display = 'none';
    document.getElementById('FramePanelPopup').style.display = 'none';
}


function replaceAll(source,stringToFind,stringToReplace)
{
    return source.split(stringToFind).join(stringToReplace);
}

function decodeUrl(inStr)
{
    var urlStr = replaceAll(inStr, '%25', '%'); 
    urlStr = replaceAll(urlStr, '%2F', '/'); 
    urlStr = replaceAll(urlStr, '%23', '#'); 
    urlStr = replaceAll(urlStr, '%3F', '?'); 
    urlStr = replaceAll(urlStr, '%26', '&'); 
    urlStr = replaceAll(urlStr, '%2C', ','); 
    urlStr = replaceAll(urlStr, '%3D', '='); 
    urlStr = replaceAll(urlStr, '%40', '@'); 
    urlStr = replaceAll(urlStr, '%3B', ';'); 
    urlStr = replaceAll(urlStr, '%24', '$'); 
    urlStr = replaceAll(urlStr, '%2B', '+'); 
    urlStr = replaceAll(urlStr, '%20', ' ');
    urlStr = replaceAll(urlStr, '%3E', '>');
    urlStr = replaceAll(urlStr, '%3C', '<');
    urlStr = replaceAll(urlStr, '%3A', ':');
    urlStr = replaceAll(urlStr, '%5B', '[');
    return replaceAll(urlStr, '%5D', ']');
}





 //////////////////////////////////////////////////////////////////////////

/**
*  Class Signal conytains Signal-related information including color and marker mode
**/
var PLOT_LINE = 1;
var PLOT_POINT = 2;
var PLOT_LINE_POINT = 3;

function Signal(x, y, color, mode, expr, shot)
{
    this.x = x;
    this.y = y;
    this.expr = expr;
    this.shot = shot;
    this.size = x.length;
    if(this.size > y.length)
        this.size = y.length;
    if(color == undefined)
        this.color = 'black';
    else
        this.color = color;
    if(this.color == 'MediumSlate')  //Supported by X11 but not by SVG
        this.color = 'blue';
    if(mode == undefined)
        this.mode = PLOT_LINE;
    else
        this.mode = mode;
    
    function getClosestIdx(x)
    {
        var idx;
        if(x <= this.x[0])
            return 0;
        if(x >= this.x[this.size - 1])
            return this.size - 1;
        for(idx = 0; idx < this.size - 1; idx++)
            if(x >= this.x[idx] && x <= this.x[idx+1])
                return idx;
    }
    this.getClosestIdx = getClosestIdx;
    
    function getYDist(idx, y)
    {
        return Math.abs(y - this.y[idx]);
    }
    this.getYDist = getYDist;
    
    function getMinX()
    {
        var idx;
        var xmin = this.x[0];
        for(idx = 0; idx < this.size; idx++)
        {
            if(this.x[idx] < xmin)
                xmin = this.x[idx]+0.;
        }
        return xmin;
    }
    this.getMinX = getMinX;
    function getMaxX()
    {
        var idx;
        var xmax = this.x[0];
        for(idx = 0; idx < this.size; idx++)
        {
            if(this.x[idx] > xmax)
                xmax = this.x[idx]+0.;
        }
        return xmax;
    }
    this.getMaxX = getMaxX;
    function getMinY()
    {
        var idx;
        var ymin = this.y[0];
        for(idx = 0; idx < this.size; idx++)
        {
            if(this.y[idx] < ymin)
                ymin = this.y[idx]+0.;
        }
        return ymin;
    }
    this.getMinY = getMinY;
    function getMaxY()
    {
        var idx;
        var ymax = this.y[0];
        for(idx = 0; idx < this.size; idx++)
        {
            if(this.y[idx] > ymax)
                ymax = this.y[idx]+0.;
        }
        return ymax;
    }
    this.getMaxY = getMaxY;
    function getMinYInRange(x1, x2)
    {
        var idx;
        for(idx = 0; idx < this.size && this.x[idx] < x1; idx++);
        if(idx == this.size) return 0; //Default interval 0-1 returned when outside signals
        var ymin = this.y[idx];
        for(; idx < this.size && this.x[idx] <= x2; idx++)
        {
            if(this.y[idx] < ymin)
                ymin = this.y[idx];
        }
        return ymin;
        
    }
    this.getMinYInRange = getMinYInRange;
    function getMaxYInRange(x1, x2)
    {
        var idx;
        for(idx = 0; idx < this.size && this.x[idx] < x1; idx++);
        if(idx == this.size) return 1; //Default interval 0-1 returned when outside signals
        var ymax = this.y[idx];
        for(; idx < this.size && this.x[idx] <= x2; idx++)
        {
            if(this.y[idx] > ymax)
                ymax = this.y[idx];
        }
        return ymax;
        
    }
    this.getMaxYInRange = getMaxYInRange;
    function append(otherSignal)
    {
        if(otherSignal.size == 0)
            return;
        var newX = eval('new ' + this.x.constructor.name + '(' + (this.size + otherSignal.size) + ')');
        newX.set(this.x);
        newX.set(otherSignal.x, this.size);
        this.x = newX;
        var newY = eval('new ' + this.y.constructor.name + '(' + (this.size + otherSignal.size) + ')');
        newY.set(this.y);
        newY.set(otherSignal.y, this.size);
        this.y = newY;
        this.size = this.x.length;
    }
    this.append = append;
    // Appends another signal at the end of this signal and removes from the beginning of this signal
    // a number of elements equal to the other signal's size
    function appendAndShift(otherSignal)
    {
        if(otherSignal.size == 0)
            return;
        if(otherSignal.size < this.size)
        {
            this.x.set(this.x.subarray(otherSignal.size));
            this.x.set(otherSignal.x, this.size - otherSignal.size);
            this.y.set(this.y.subarray(otherSignal.size));
            this.y.set(otherSignal.y, this.size - otherSignal.size);
        }
        else
        {
            this.x = otherSignal.x;
            this.y = otherSignal.y;
            this.size = otherSignal.size;
        }
    }
    this.appendAndShift = appendAndShift;
}
 
/**
*  Class Metrics handles mapping between waveform coordinates and pixel coordinates
**/
function Metrics(marginPix, width, height, xMin, xMax, yMin, yMax)
{
    this.width = width;
    this.height = height;
    this.marginPix = marginPix;
    this.xmin = xMin;
    this.xmax = xMax;
    this.ymin = yMin;
    this.ymax = yMax;
    
   //if no limuts passed provide default limits
    if (this.xmin == undefined && this.xmax == undefined)
    {
        this.xmin = 0.; 
        this.xmax = 1;    
    } 
    else
    {
        if(this.xmax == undefined)
            this.xmax = this.xmin + 1.;
        if(this.xmin == undefined)
            this.xmin = this.xmax - 1.;
    }
    if (this.ymin == undefined && this.ymax == undefined)
    {
        this.ymin = 0.; 
        this.ymax = 1;    
    } 
    else
    {
        if(this.ymax == undefined)
            this.ymax = this.ymin + 1.;
        if(this.ymin == undefined)
            this.ymin = this.ymax - 1.;
    }
    if(this.ymin >= this.ymax)
        this.ymax = this.ymin + 1E-3;
    this.xfact = (this.width - 2 * this.marginPix)/(this.xmax - this.xmin);
    this.yfact = (this.height - 2 * this.marginPix)/(this.ymax - this.ymin);

   
    function getXPixel(xVal)
    {
        var xPixel = Math.round(this.marginPix + this.xfact * (xVal - this.xmin));
    return xPixel;
        if(xPixel >= 0 && xPixel <= this.width)
            return xPixel;
        else
            return undefined;
    }
    this.getXPixel = getXPixel;
    function getYPixel(yVal)
    {
        var yPixel = Math.round(this.height -this.marginPix - this.yfact * (yVal - this.ymin)); 
        if(isNaN(yPixel))
          return undefined;
        else
      return yPixel;
    }
    this.getYPixel = getYPixel;
    function getXValue(xPixel)
    {
        return this.xmin + (xPixel - this.marginPix)/this.xfact;
    }
    this.getXValue = getXValue;
    function getYValue(yPixel)
    {
        return this.ymin + (this.height - yPixel - this.marginPix)/this.yfact;
    }
    this.getYValue = getYValue;
}

function Grid(g, metrics, labels, xIsDateTime)
{
    this.xmin = metrics.xmin;
    this.xmax = metrics.xmax;
    this.ymin = metrics.ymin;
    this.ymax = metrics.ymax;
    this.width = metrics.width;
    this.height = metrics.height;
    this.metrics = metrics;
    this.labels = labels;
    this.g = g;
    this.xIsDateTime = xIsDateTime;
    
    function buildGrid(minValue, maxValue, numSteps)
    {
        var i;
        if (maxValue < minValue)
            maxValue = minValue + 1E-10;

        var range = maxValue - minValue;
        if (range <= 0)
            range = 1E-3;

        var currMaxValue = maxValue;// + 0.1 * range;
        var currMinValue = minValue;// - 0.1 * range;
        var step = (maxValue - minValue) / numSteps;

        var greater;
        var count = 0;
        if (step > 1)
        {
            greater = true;
            while (step / 10. > 1.)
            {
                step /= 10;
                count++;
            }
        }
        else
        {
            greater = false;
            while (step > 0 && step < 1)
            {
                step *= 10;
                count++;
            }
        }
        step = Math.round(step);
        if (greater)
            for (i = 0; i < count; i++)
                step *= 10;
        else
            for (i = 0; i < count; i++)
                step /= 10;
        
        var curr = Math.round(currMinValue / step) * step;
       // while (curr >= currMinValue)
       //     curr -= step;
        
        var retVals = new Array();
//        while(curr < currMaxValue + step)
        while(curr < currMaxValue)
        {
            var currVal = Math.round(curr / step) * step;
//Fix for 0 value
            if (currVal < step / 100. && currVal > -step / 100.)
                currVal = 0;
            retVals.push(currVal);
            curr += step;
        }
        return retVals;
    }
    this.buildGrid = buildGrid;

    if(this.xIsDateTime)
        this.xVals = this.buildGrid(this.xmin, this.xmax, 6);
    else
        this.xVals = this.buildGrid(this.xmin, this.xmax, 8);
    this.yVals = this.buildGrid(this.ymin, this.ymax, 5);
    
    function roundLabel(label)
    {
 //       var dotPos = label.indexOf(".");
 //       if(dotPos == -1 || (label.length - dotPos) < 8)
 //           return label;
        var rounded;
        if(Math.abs(label) < 1E10)
             rounded = Math.round(parseFloat(label)*100000)/100000;
        else
             rounded = Math.round(parseFloat(label)*10)/10;
        if((Math.abs(rounded) > 0.0001 && Math.abs(rounded) < 10000)||rounded == 0)
            return ''+rounded;
        else
            return rounded.toExponential();
            
    }
    this.roundLabel = roundLabel;

    function plot()
    {
        var prevDateString = "";
        var plotDates = false;
        if(this.xIsDateTime)
        {
            var firstDateString = dateTimeFormatYMD.format(this.xVals[0]);
            var lastDateString = dateTimeFormatYMD.format(this.xVals[this.xVals.length - 1]);
            plotDates = (firstDateString != lastDateString);
        }
        for (var idx=0; idx < this.xVals.length; idx++) {
            if(this.metrics.getXPixel(this.xVals[idx]) > this.width / 30)
            {
                var label=document.createElementNS("http://www.w3.org/2000/svg","text");
                this.g.appendChild(label);
                label.setAttributeNS(null,"text-anchor","start");
                //label.setAttributeNS(null,"x",this.xScaling[1]+idx * this.xScaling[2]);
                label.setAttributeNS(null,"x",this.metrics.getXPixel(this.xVals[idx]));
                if(this.labels != undefined && this.labels.xlabel != undefined)
                    label.setAttributeNS(null,"y",this.height - 20);
                else
                    label.setAttributeNS(null,"y",this.height - 10);
                label.setAttributeNS(null,"font-size","16px");
                var textValue;
                if(this.xIsDateTime)
                {
                    var xDateTime = new Date(this.xVals[idx]);
                    textValue = dateTimeFormatHMS.format(xDateTime);
                    if(idx > 0 && this.xVals[idx] - this.xVals[idx - 1] < 1000)
                        textValue += "." + xDateTime.getMilliseconds();
                    var currDateString = dateTimeFormatYMD.format(xDateTime);
                    if(plotDates && currDateString != prevDateString)
                    {
                        var dateLabel = document.createElementNS("http://www.w3.org/2000/svg","text");
                        this.g.appendChild(dateLabel);
                        dateLabel.setAttributeNS(null,"text-anchor","start");
                        dateLabel.setAttributeNS(null,"x",this.metrics.getXPixel(this.xVals[idx]));
                        if(this.labels.xlabel != undefined)
                            dateLabel.setAttributeNS(null,"y",this.height - 35);
                        else
                            dateLabel.setAttributeNS(null,"y",this.height - 25);
                        dateLabel.setAttributeNS(null,"font-size","16px");
                        dateLabel.appendChild(document.createTextNode(currDateString));
                        prevDateString = currDateString;

                        var dateSeparationValue = new Date(xDateTime.getFullYear(), xDateTime.getMonth(), xDateTime.getDate()).getTime();
                        var dateSeparationLine = document.createElementNS("http://www.w3.org/2000/svg","line");
                        dateSeparationLine.setAttribute("y1","0");
                        dateSeparationLine.setAttribute("y2",""+this.height);
                        dateSeparationLine.setAttribute("x1",this.metrics.getXPixel(dateSeparationValue));
                        dateSeparationLine.setAttribute("x2",this.metrics.getXPixel(dateSeparationValue));
                        dateSeparationLine.setAttribute("stroke","blue");
                        dateSeparationLine.setAttribute("stroke-width","1");
                        dateSeparationLine.setAttribute("stroke-dasharray", "4,8");
                        g.appendChild(dateSeparationLine);
                    }
                }
                else
                    textValue = this.roundLabel(this.xVals[idx]+"");
                var textNode=document.createTextNode(textValue);
                label.appendChild(textNode);
            }
            
            var lineY=document.createElementNS("http://www.w3.org/2000/svg","line");
            lineY.setAttribute("y1","0");
            lineY.setAttribute("y2",''+this.height);
 //           lineY.setAttribute("x1",this.width-(this.xScaling[1]+idx * this.xScaling[2]));
 //           lineY.setAttribute("x2",this.width-(this.xScaling[1]+idx * this.xScaling[2]));
            lineY.setAttribute("x1",this.metrics.getXPixel(this.xVals[idx]));
            lineY.setAttribute("x2",this.metrics.getXPixel(this.xVals[idx]));
            lineY.setAttribute("stroke","black");
            lineY.setAttribute("stroke-width","1");
            lineY.setAttribute("stroke-dasharray", "1,4");
            g.appendChild(lineY);
        }
        for (var idx=0; idx < this.yVals.length; idx++) {
            var label=document.createElementNS("http://www.w3.org/2000/svg","text");
            this.g.appendChild(label);
            label.setAttributeNS(null,"text-anchor","start");
            if(this.labels != undefined && this.labels.ylabel != null)
                label.setAttributeNS(null,"x",18);
            else
                label.setAttributeNS(null,"x",10);
            label.setAttributeNS(null,"y",this.metrics.getYPixel(this.yVals[idx]));
            var fontSize = this.height / this.yVals.length;
            if(fontSize > 16)
                fontSize = 16;
            if(fontSize < 2)
                fontSize = 2;
            label.setAttributeNS(null,"font-size",fontSize+"px");
//            label.setAttributeNS(null,"font-size","16px");
            label.setAttribute("alignment-baseline", "middle");
            //var value=Round(this.yScaling[3]+idx * this.yScaling[4], this.yScaling[5]);
            var textNode=document.createTextNode(this.roundLabel(this.yVals[idx]+''));
            //var textNode=document.createTextNode(value.toFixed(9));
             label.appendChild(textNode);
            var lineX=document.createElementNS("http://www.w3.org/2000/svg","line");
            lineX.setAttribute("x1","0");
            lineX.setAttribute("x2",''+this.width);
//            lineX.setAttribute("y1",this.height-(this.yScaling[1]+idx * this.yScaling[2]));
//            lineX.setAttribute("y2",this.height-(this.yScaling[1]+idx * this.yScaling[2]));
            lineX.setAttribute("y1",this.metrics.getYPixel(this.yVals[idx]));
            lineX.setAttribute("y2",this.metrics.getYPixel(this.yVals[idx]));
            lineX.setAttribute("stroke","black");
            lineX.setAttribute("stroke-width","1");
            lineX.setAttribute("stroke-dasharray", "1,4");
            g.appendChild(lineX);
        }
        if(this.labels != undefined && (this.labels.title != undefined || this.labels.errors != undefined))
        {
            var title=document.createElementNS("http://www.w3.org/2000/svg","text");
            this.g.appendChild(title);
            title.setAttributeNS(null,"text-anchor","middle");
            title.setAttributeNS(null,"x",this.metrics.width/2);
            title.setAttributeNS(null,"y",18);
            title.setAttributeNS(null,"font-size","14px");
            /*if(this.labels.title.length < 20)
             else if(this.labels.title.length < 40)
             title.setAttributeNS(null,"font-size","12px");
            else
             title.setAttributeNS(null,"font-size","8px"); */
            //title.setAttributeNS(null,"alignment-baseline", "text-after-edge");
            if(this.labels.errors != undefined)
		title.appendChild(document.createTextNode(this.labels.errors));
	    else
		title.appendChild(document.createTextNode(this.labels.title));
	    if(this.labels.errors != undefined)
            {
                var errorLabel=document.createElementNS("http://www.w3.org/2000/svg","text");
                var tooltip=document.createElementNS("http://www.w3.org/2000/svg","title");
                errorLabel.appendChild(tooltip);
                this.g.appendChild(errorLabel);
                errorLabel.setAttributeNS(null,"text-anchor","middle");
                errorLabel.setAttributeNS(null,"x",this.metrics.width/2);
                errorLabel.setAttributeNS(null,"y",36);
                errorLabel.setAttributeNS(null,"font-size","14px");
 //               var parsedMessage = this.labels.errors.match(/u'%[A-Za-z\-\,\s_]+/);
                var parsedMessage = this.labels.errors;
                var errorMessage = '';
		try {
                  if(parsedMessage != undefined);
                    errorMessage = parsedMessage[0].substring(3);
                //var errorMessage=this.labels.errors.match(/u'%[A-Za-z\-\,\s_]+/)[0].substring(3);
                  errorLabel.appendChild(document.createTextNode(errorMessage));
                  tooltip.appendChild(document.createTextNode(errorMessage));
		}catch(err) {}

            }
        }
        if(this.labels != undefined && this.labels.ylabel != undefined)
        {
            var ylabel=document.createElementNS("http://www.w3.org/2000/svg","text");
            this.g.appendChild(ylabel);
            ylabel.setAttributeNS(null,"text-anchor","middle");
            ylabel.setAttributeNS(null,"y",this.metrics.height/2);
            ylabel.setAttributeNS(null,"x",0);
            ylabel.setAttribute("transform", "rotate(270, "+16+","+this.metrics.height/2+")"), 
            ylabel.setAttributeNS(null,"font-size","16px");
            //title.setAttributeNS(null,"alignment-baseline", "text-after-edge");
            ylabel.appendChild(document.createTextNode(this.labels.ylabel));
        }
        if(this.labels != undefined && this.labels.xlabel != undefined)
        {
            var xlabel=document.createElementNS("http://www.w3.org/2000/svg","text");
            this.g.appendChild(xlabel);
            xlabel.setAttributeNS(null,"text-anchor","middle");
            xlabel.setAttributeNS(null,"y",this.metrics.height - 5);
            xlabel.setAttributeNS(null,"x",this.metrics.width/2);
             xlabel.setAttributeNS(null,"font-size","16px");
            //title.setAttributeNS(null,"alignment-baseline", "text-after-edge");
            xlabel.appendChild(document.createTextNode(this.labels.xlabel));
        }
    }
    this.plot = plot;
}


function Wave(signals, color, g, metrics, clippath)
{
    this.signals = signals;
    this.g = g;
    this.color = color;
    this.metrics = metrics;
    this.clippath = clippath;
    function plot()
    {
        var idx;
        var signalIdx;
        var clip_path_id=this.clippath.getAttribute("id");
        this.g.setAttribute("clip-path","url(#"+clip_path_id+")");
        var numSignals;
        if(this.signals == undefined)
            numSignals = 0;
        else
            numSignals = this.signals.length;
        for(signalIdx = 0; signalIdx < numSignals; signalIdx++)
        {
            if(this.signals[signalIdx].mode == PLOT_LINE || this.signals[signalIdx].mode == PLOT_LINE_POINT)
            {    
                var path=document.createElementNS("http://www.w3.org/2000/svg","path");
                path.setAttribute("fill","none");
                path.setAttribute("stroke",this.signals[signalIdx].color);
                path.setAttribute("stroke-width","1");
                g.appendChild(path);
                this.path = path;
                var ans='M';
                var prevSkipped = false;
                var prevXs = this.metrics.getXPixel(this.signals[signalIdx].x[0]);
                var minYs = this.metrics.getYPixel(this.signals[signalIdx].y[0]);
                var maxYs = minYs;
                var lastYs;
                var firstYs = minYs;
                for (idx=0; idx < this.signals[signalIdx].size; idx++) {
                    var xs=this.metrics.getXPixel(this.signals[signalIdx].x[idx]);
                    var ys=this.metrics.getYPixel(this.signals[signalIdx].y[idx]);
                    if(ys == undefined || xs == undefined) continue;
                    if(idx > 0 && xs == prevXs)
                    {
                        if(ys < minYs) minYs = ys;
                        if(ys > maxYs) maxYs = ys;
                        lastYs = ys;
                    }
                    else
                    {
                        if(minYs == undefined)
                        {
                            minYs = ys;
                            maxYs = ys;
                        }
                        if(firstYs != minYs)
                        {
                               ans = ans + ' ' + prevXs + ' ' +firstYs;
                        }
                        ans = ans + ' ' + prevXs + ' ' +minYs;
                        if (maxYs > minYs)
                        {
                            ans = ans + ' ' + prevXs + ' ' +maxYs;
                           if(lastYs != maxYs)
                           {
                               ans = ans + ' ' + prevXs + ' ' +lastYs;
                           }
                        }
                        minYs = ys;
                        maxYs = ys;
                        firstYs = ys;
                        lastYs = ys;
                        prevXs = xs;
                        
                    } 

                    //Draw last point 
                    if(idx == this.signals[signalIdx].size - 1)
                    {
                        ans = ans + ' ' + xs + ' ' +ys;
                    }
/*                    if((xs == undefined) || (ys == undefined))
                        prevSkipped = true;
                    else
                    {
                        if(prevSkipped && ans != 'M')
                            ans = ans + " M " + xs + " " + ys;
                        else
                            ans = ans + " " + xs + " " + ys;
                        prevSkipped = false;
                    }
 
*/              }
                path.setAttribute("d",ans);
            }
            if(this.signals[signalIdx].mode == PLOT_POINT || this.signals[signalIdx].mode == PLOT_LINE_POINT)
            {  
                for (idx=0; idx < this.signals[signalIdx].size; idx++) {
                    var xs=this.metrics.getXPixel(this.signals[signalIdx].x[idx]);
                    var ys=this.metrics.getYPixel(this.signals[signalIdx].y[idx]);
                    if((xs != undefined) && (ys != undefined))
                    {
                        var circle=document.createElementNS("http://www.w3.org/2000/svg","circle");
                        circle.setAttribute("cx",xs);
                        circle.setAttribute("cy",ys);
                        circle.setAttribute("r",2);
                        circle.setAttribute("fill",this.signals[signalIdx].color);
                        g.appendChild(circle);
                    }
                }
            }  
        }
    }
    this.plot = plot;
    
    
    function getCrosshairIdx(xVal, y)
    {
        var minSigIdx;
        var minDist;
        var idx;
        for(idx = 0; idx < this.signals.length; idx++)
        {
            var currMinIdx = this.signals[idx].getClosestIdx(xVal);
            var currDist = this.signals[idx].getYDist(currMinIdx, y);
            if(minDist == undefined || minDist > currDist)
            {
                minSigIdx = idx;
                minDist = currDist;
            }
        }
        return minSigIdx;
    }
    this.getCrosshairIdx = getCrosshairIdx;

    function getCrosshairY(xVal, sigIdx)
    {
        return this.signals[sigIdx].y[this.signals[sigIdx].getClosestIdx(xVal)];
    }
    this.getCrosshairY = getCrosshairY;
    function getCrosshairColor(sigIdx)
    {
        if(this.signals == undefined || sigIdx >= this.signals.length)
            return "Black";
        return this.signals[sigIdx].color;
    }
    this.getCrosshairColor = getCrosshairColor;
    function getCrosshairExpr(sigIdx)
    {
        return signals[sigIdx].expr;
    }
    this.getCrosshairExpr = getCrosshairExpr;
    function getCrosshairShot(sigIdx)
    {
        return signals[sigIdx].shot;
    }
    this.getCrosshairShot = getCrosshairShot;
    function getMinX()
    {
        if (signals == undefined || signals.size == 0|| signals[0] == undefined) return 0;
        var xmin = this.signals[0].getMinX();
        for(var idx = 0; idx < this.signals.size; idx++)
        {
            var currVal = this.signals[idx].getMinX();
            if(currVal < xmin)
                xmin = currVal;
        }
        return xmin;
    }
    this.getMinX = getMinX;
    function getMaxX()
    {
        if (signals == undefined || signals.size == 0 || signals[0] == undefined) return 1;
        var xmax = this.signals[0].getMaxX();
        for(var idx = 0; idx < this.signals.size; idx++)
        {
            var currVal = this.signals[idx].getMaxX();
            if(currVal > xmax)
                xmax = currVal;
        }
        return xmax;
    }
    this.getMaxX = getMaxX;
    
}    
 
function WavePanel(svg, panelIdx, numCols, numRows, col, row, clippath, tree, shots, colors, modes, signalExprs, limitExprs, labelExprs, defaultNodeExpr, continuousUpdate)
{
    this.STARTING_ZOOM = 1;
    this.ZOOMING = 2;
    this.NOT_ZOOMING = 3;
    this.DEFAULT_NUM_SAMPLES = Math.round(1500/numCols);
    this.CONTIUNUOUS_UPDATE_WHOLE_WAVE = 1;
    this.CONTIUNUOUS_UPDATE_END_OF_WAVE = 2;
    this.zoomState = this.NOT_ZOOMING;
    this.svg = svg;
    this.numCols = numCols;
    this.numRows = numRows;
    this.col = col;
    this.row = row;
    this.panelIdx = panelIdx;
    this.clippath = clippath;
    this.zoomCache = new Array();
    this.updating = false;
    this.xIsDateTime = false;

    this.borderRect=document.createElementNS("http://www.w3.org/2000/svg","rect");
    this.borderRect.setAttribute("x","0");
    this.borderRect.setAttribute("y","0");
    this.borderRect.setAttribute("width",svg.viewBox.baseVal.width);
    this.borderRect.setAttribute("height",svg.viewBox.baseVal.height);
    this.borderRect.setAttribute("fill","none");
    this.borderRect.setAttribute("stroke","black");
    svg.appendChild(this.borderRect);
  
  //Configuration definition
    this.tree = tree;
    this.shots = shots;
    this.colors = colors;
    this.modes = modes;
    this.signalExprs = signalExprs;
    this.limitExprs = limitExprs;
    this.labelExprs = labelExprs;
    this.defaultNodeExpr = defaultNodeExpr;

    function enableMouseMove()
    {
        this.svg.addEventListener('mousemove', this.mouseMove = function(evt) {
            mouseMove(evt);
        }, false);
        //var childNodes = this.g.childNodes;
        //for(var i = 0; i < childNodes.length; i++)
        //    childNodes[i].addEventListener('mousemove', this.mouseMove = function(evt) {
        //        }, true);
    }
    this.enableMouseMove = enableMouseMove;
    
    function disableMouseMove()
    {
        this.svg.removeEventListener('mousemove', this.mouseMove, false);
    }
    this.disableMouseMove = disableMouseMove;
     
    function update(minRequestedX, maxRequestedX, numSamples, continuousUpdateCase)
    {
        this.updating = true;
        if(minRequestedX == undefined)
        {
            if(this.metrics != undefined)
                minRequestedX = this.metrics.xmin;
            else
                minRequestedX = -Number.MAX_VALUE;
        }
        if(maxRequestedX == undefined)
        {
            if(this.metrics != undefined)
                maxRequestedX = this.metrics.xmax;
            else
                maxRequestedX = Number.MAX_VALUE;
        }
        if(this.xIsDateTime)
        {
            minRequestedX = Math.floor(minRequestedX) + 'QU';
            maxRequestedX = Math.ceil(maxRequestedX) + 'QU';
        }
        else
        {
            // minRequestedX and maxRequestedX must contain a decimal point, otherwise the request fails
            if(minRequestedX.toString().indexOf('.') == -1 && minRequestedX.toString().indexOf('e') == -1)
                minRequestedX = minRequestedX + '.';
            if(maxRequestedX.toString().indexOf('.') == -1 && maxRequestedX.toString().indexOf('e') == -1)
                maxRequestedX = maxRequestedX + '.';
        }
        if(numSamples == undefined)
            numSamples = this.DEFAULT_NUM_SAMPLES;
        var req = new XMLHttpRequest();
        var getStr = 'scope?panel=yes';
        if(continuousUpdateCase == undefined)
        {
            var updateFunction = 'MdsMisc->GetXYWave:DSC';
            if(this.xIsDateTime)
                updateFunction = 'MdsMisc->GetXYWaveLongTimes:DSC';
            for(var i = 0; i < this.signalExprs.length; i++)
            {
                getStr = getStr + '&y' + (i+1) + '=' + updateFunction + '('
                                + '\'' + replaceAll(replaceAll(this.signalExprs[i], '\\', '\\\\'), '\'', '\\\'') + '\','
                                + encodeURIComponent(minRequestedX) + ',' + encodeURIComponent(maxRequestedX) + ',' + numSamples + ')';
            }
        }
        else
        {
            getStr = getStr + '&continuous_update=yes&displayed_width=' + (this.metrics.xmax - this.metrics.xmin) + '&default_num_samples=' + this.DEFAULT_NUM_SAMPLES;
            for(var i = 0; i < this.signalExprs.length; i++)
            {
                getStr = getStr + '&y' + (i+1) + '=' + this.signalExprs[i] + '&curr_max_x' + (i+1) + '=' + this.signals[i].getMaxX();
            }
        }
        if(this.tree != undefined)
        {
            getStr = getStr + '&tree=' + this.tree;
            if(this.shots != undefined && this.shots.length > 0)
            {
                getStr = getStr + '&shot=' + this.shots.join(',');
            }
        }
        if(this.labelExprs.title != undefined)
             getStr = getStr + '&title='+labelExprs.title;
        if(this.labelExprs.xLabel != undefined)
             getStr = getStr + '&xlabel='+labelExprs.xLabel;
        if(this.labelExprs.yLabel != undefined)
             getStr = getStr + '&ylabel='+labelExprs.yLabel;
        if(this.limitExprs.xMin != undefined)
             getStr = getStr + '&xmin='+this.limitExprs.xMin;
        if(this.limitExprs.xMax != undefined)
             getStr = getStr + '&xmax='+this.limitExprs.xMax;
        if(this.limitExprs.yMin != undefined)
             getStr = getStr + '&ymin='+this.limitExprs.yMin;
        if(this.limitExprs.yMax != undefined)
             getStr = getStr + '&ymax='+this.limitExprs.yMax;

        req.open('GET', getStr, true);
        req.wavePanel = this;
        req.numSignals = this.signalExprs.length;
        req.continuousUpdateCase = continuousUpdateCase;
        if(this.shots == undefined)
            req.numShots = 0;
        else
            req.numShots = this.shots.length;
        req.onreadystatechange = function() {
            if (req.readyState == 4) 
            {
                if (this.getResponseHeader('ERROR'))
                    this.wavePanel.updateFailure(this.getResponseHeader('ERROR'));
                else
                {
                    var signals = new Array();
                    var errors = '';
                    var signalsHaveChanged = false;
                    if(this.numShots <= 1)
                    {
                        var currOffset = 0;
                        for(var sigIdx = 0; sigIdx < this.numSignals; sigIdx++)
                        {
                            var currError = this.getResponseHeader('ERROR'+(sigIdx + 1));
                            if(currError != undefined)
                                errors = errors + '\n'+currError;
                            else
                            {
                                var xdtype = this.getResponseHeader('X'+(sigIdx + 1)+'_DATATYPE');
                                var ydtype = this.getResponseHeader('Y'+(sigIdx + 1)+'_DATATYPE');
                                var xlength = parseInt(this.getResponseHeader('X'+(sigIdx + 1)+'_LENGTH'));
                                var ylength = parseInt(this.getResponseHeader('Y'+(sigIdx + 1)+'_LENGTH'));
                                var x = eval('new '+xdtype+'(this.response,currOffset,xlength)');
                                var y = eval('new '+ydtype+'(this.response,currOffset + xlength*x.BYTES_PER_ELEMENT,ylength)');
                                currOffset = currOffset + xlength*x.BYTES_PER_ELEMENT+ylength*y.BYTES_PER_ELEMENT;
                                var shotStr = '';
                                if(this.numShots == 1)
                                    shotStr = ''+this.wavePanel.shots[0];
                                if(xlength == 0)
                                    signals.push(this.wavePanel.signals[sigIdx]);  // This signal hasn't changed, keep the current data
                                else
                                {
                                    signals.push(new Signal(x, y, this.wavePanel.colors[sigIdx], this.wavePanel.modes[sigIdx], decodeUrl(this.wavePanel.signalExprs[sigIdx]), shotStr));
                                    signalsHaveChanged = true;
                                }
                            }
                        }
                    }  
                    else
                    {
                        var currSigIdx = 0;
                        var currOffset = 0;
                        for(var shotIdx = 0; shotIdx < this.numShots; shotIdx++)
                        {
                            for(var sigIdx = 0; sigIdx < this.numSignals; sigIdx++)
                            {
                                var currError = this.getResponseHeader('ERROR'+(currSigIdx + 1));
                                if(currError != undefined)
                                    errors = errors + '\n'+currError;
                                else
                                {
                                    var xdtype = this.getResponseHeader('X'+(currSigIdx + 1)+'_DATATYPE');
                                    var ydtype = this.getResponseHeader('Y'+(currSigIdx + 1)+'_DATATYPE');
                                    var xlength = parseInt(this.getResponseHeader('X'+(currSigIdx + 1)+'_LENGTH'));
                                    var ylength = parseInt(this.getResponseHeader('Y'+(currSigIdx + 1)+'_LENGTH'));
                                    var x = eval('new '+xdtype+'(this.response,currOffset,xlength)');
                                    var y = eval('new '+ydtype+'(this.response,currOffset + xlength*x.BYTES_PER_ELEMENT,ylength)');
                                    currOffset = currOffset + xlength*x.BYTES_PER_ELEMENT+ylength*y.BYTES_PER_ELEMENT;
                                    var shotStr = ''+this.wavePanel.shots[shotIdx];
                                    if(xlength == 0)
                                        signals.push(this.wavePanel.signals[sigIdx]);  // This signal hasn't changed, keep the current data
                                    else
                                    {
                                        signals.push(new Signal(x, y, colorPalette[currSigIdx%colorPalette.length], this.wavePanel.modes[sigIdx], decodeUrl(this.wavePanel.signalExprs[sigIdx]), shotStr));
                                        signalsHaveChanged = true;
                                    }
                                }
                                currSigIdx++;
                            }
                        }
                    }
                    if(this.continuousUpdateCase != undefined && !signalsHaveChanged)
                    {
                        this.wavePanel.updating = false;
                        return;
                    }

                    var setting;
                    var limits = new Object();
                    if((setting = this.getResponseHeader('xmin')) != null)
                        limits.xmin = parseFloat(setting);
                    if((setting = this.getResponseHeader('xmax')) != null)
                        limits.xmax = parseFloat(setting);
                    if((setting = this.getResponseHeader('ymin')) != null)
                        limits.ymin = parseFloat(setting);
                    if((setting = this.getResponseHeader('ymax')) != null)
                        limits.ymax = parseFloat(setting);
                    
                    var labels = new Object();
                    if((setting = this.getResponseHeader('title')) != null)
                        labels.title = setting;
                    if((setting = this.getResponseHeader('xlabel')) != null)
                        labels.xlabel = setting;
                    if((setting = this.getResponseHeader('xlabel')) != null)
                        labels.ylabel = setting;
                    if(this.getResponseHeader('X_IS_DATETIME') != null)
                        this.wavePanel.xIsDateTime = true;
                    if(errors != '')
                        labels.errors = errors;
                    
                    this.wavePanel.updateSuccess(signals, labels, limits, this.continuousUpdateCase);
                }
            }
        };
        req.responseType = 'arraybuffer';
        req.send();
    }
    this.update = update;

    function updateSuccess(signals, labels, limits, continuousUpdateCase)
    {
        if(signals.length == 0 && (labels.errors == undefined || labels.errors == ''))
	    return;
        var isFirstUpdate = false;
        if(this.originalSignals == undefined && continuousUpdateCase == undefined)
        {
            isFirstUpdate = true;
            this.originalSignals = signals;
        }
        this.setPanel(signals, labels, limits, continuousUpdateCase, isFirstUpdate);
        this.updating = false;
    }
    this.updateSuccess = updateSuccess;

    function updateFailure(error)
    {
        alert('Error getting signals for panel ('+this.row+', '+this.col+'): '+error);
        this.updating = false;
    }
    this.updateFailure = updateFailure;
    
    function plot()
    {
        this.grid.plot();
        this.wave.plot();
    }
    this.plot = plot;

    function setPanel(signals, labels, limits, continuousUpdateCase, isFirstUpdate)
    {
        if(this.g != undefined)
            this.svg.removeChild(this.g);
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.svg.appendChild(this.g);

        if(continuousUpdateCase == this.CONTIUNUOUS_UPDATE_WHOLE_WAVE)
        {
            for(var idx = 0; idx < this.signals.length; idx++)
                this.signals[idx].append(signals[idx]);
        }
        else if(continuousUpdateCase == this.CONTIUNUOUS_UPDATE_END_OF_WAVE)
        {
            for(var idx = 0; idx < this.signals.length; idx++)
                this.signals[idx].appendAndShift(signals[idx]);
        }
        else
            this.signals = signals;

        this.limits = limits;
        this.labels = labels;

    //find min and max X and Y values of the set of signals
        if(continuousUpdateCase != undefined || this.xMin == undefined || this.xMax == undefined || this.yMin == undefined || this.yMax == undefined)
        {
            for(var sigIdx = 0; sigIdx < signals.length; sigIdx++)
            {
                var currXMin = this.signals[sigIdx].getMinX();
                if(this.xMin == undefined || currXMin < this.xMin)
                    this.xMin = currXMin;
                var currXMax = this.signals[sigIdx].getMaxX();
                if(this.xMax == undefined || currXMax > this.xMax)
                    this.xMax = currXMax;
                var currYMin = this.signals[sigIdx].getMinY();
                if(this.yMin == undefined || currYMin < this.yMin)
                    this.yMin = currYMin;
                var currYMax = this.signals[sigIdx].getMaxY();
                if(this.yMax == undefined || currYMax > this.yMax)
                    this.yMax = currYMax;
            }
        }

        var xMin = this.xMin;
        var xMax = this.xMax;
        var yMin = this.yMin;
        var yMax = this.yMax;
   //Override default limits with given ones
        if(isFirstUpdate && continuousUpdateCase == undefined)
        {
            if(this.limits.xmin != undefined)
                xMin = this.limits.xmin;
            if(this.limits.xmax != undefined)
                xMax = this.limits.xmax;
            if(this.limits.ymin != undefined)
                yMin = this.limits.ymin;
            if(this.limits.ymax != undefined)
                yMax = this.limits.ymax;
        }

        if(continuousUpdateCase == this.CONTIUNUOUS_UPDATE_WHOLE_WAVE || this.metrics == undefined)
        {
            this.metrics = new Metrics(METRICS_BORDER, this.svg.viewBox.baseVal.width, this.svg.viewBox.baseVal.height, 
                xMin, xMax, yMin, yMax);
        }
        else if(continuousUpdateCase == this.CONTIUNUOUS_UPDATE_END_OF_WAVE)
        {
            this.metrics = new Metrics(METRICS_BORDER, this.svg.viewBox.baseVal.width, this.svg.viewBox.baseVal.height, 
                xMax - (this.metrics.xmax - this.metrics.xmin), xMax, this.metrics.ymin, this.metrics.ymax);
        }

        var wave = new Wave(this.signals, 0, this.g, this.metrics, this.clippath);
        var grid = new Grid(this.g, this.metrics, this.labels, this.xIsDateTime);
        this.wave = wave;
        this.grid = grid;
        this.currentWaveMinX = this.wave.getMinX();
        this.currentWaveMaxX = this.wave.getMaxX();
        this.plot();
        if(isFirstUpdate && (this.limits.xmin != undefined || this.limits.xmax != undefined))
            this.update();
    }
    this.setPanel = setPanel;

    if(shots != undefined && shots[0] < -1) //No global shot defined yet
    {
        var currLabels = new Array();
        currLabels.title = 'Global shot not yet defined';
        this.setPanel(new Array(), currLabels, new Object()); 
    }
    else
    {
        this.update();
        if(continuousUpdate)
        {
            var wavePanel = this;
            setInterval(function(){
                var endOfWaveThreshold = wavePanel.xMax - (wavePanel.metrics.xmax - wavePanel.metrics.xmin)/20;
                if(!wavePanel.updating && !wavePanel.isZooming() && !wavePanel.panning)
                {
                    if((wavePanel.metrics.xmax - wavePanel.metrics.xmin) == (wavePanel.xMax - wavePanel.xMin))  // Not zoomed in
                        wavePanel.update(undefined, undefined, wavePanel.DEFAULT_NUM_SAMPLES, wavePanel.CONTIUNUOUS_UPDATE_WHOLE_WAVE);
                    else if(wavePanel.metrics.xmax >= endOfWaveThreshold)  // Zoomed in at the end of the wave
                        wavePanel.update(undefined, undefined, wavePanel.DEFAULT_NUM_SAMPLES, wavePanel.CONTIUNUOUS_UPDATE_END_OF_WAVE);
                }
            }, 1000);
        }
    }

    function removeAllSignals()
    {
        this.signals = new Array();
        this.svg.removeChild(this.g);
        this.zoomRect = undefined;
        if(this.crosshairVertLine != undefined)
        {
            this.svg.removeChild(this.crosshairVertLine);
            this.crosshairVertLine = undefined;
        }
        if(this.crosshairHorLine != undefined)
        {
            this.svg.removeChild(this.crosshairHorLine);
            this.crosshairHorLine = undefined;
        }
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.svg.appendChild(this.g);
        this.xMin = undefined;
        this.xMax = undefined;
        this.yMin = undefined;
        this.yMax = undefined;
        var metrics = new Metrics(METRICS_BORDER, svg.viewBox.baseVal.width, svg.viewBox.baseVal.height, 
            0., 1. ,0., 1.);
        var wave = new Wave(this.signals, 0, this.g, metrics, this.clippath);
        var grid = new Grid(this.g, metrics, this.labels, this.xIsDateTime);
        this.metrics = metrics;
        this.wave = wave;
        this.grid = grid;
        this.plot();
    }
    this.removeAllSignals = removeAllSignals;
    
    function resize(width, height)
    {
        this.svg.setAttributeNS(null,"width",width);
        this.svg.setAttributeNS(null,"height",height);
        this.svg.setAttribute("viewBox","0 0 "+width+" " +height);
        this.svg.removeChild(this.borderRect);
        this.borderRect=document.createElementNS("http://www.w3.org/2000/svg","rect");
        this.borderRect.setAttribute("x","0");
        this.borderRect.setAttribute("y","0");
        this.borderRect.setAttribute("width",svg.viewBox.baseVal.width);
        this.borderRect.setAttribute("height",svg.viewBox.baseVal.height);
        this.borderRect.setAttribute("fill","none");
        this.borderRect.setAttribute("stroke","black");
        svg.appendChild(this.borderRect);
        this.svg.parentNode.setAttribute("width", width);
        this.svg.parentNode.setAttribute("height", height);
        this.clippath.getElementsByTagName("path")[0].setAttribute("d","M 0 0 0 " + height + " " + width + " " + height + " " + width + " 0");
        if(this.g != undefined)
            this.svg.removeChild(this.g);
        this.zoomRect = undefined;
        if(this.crosshairVertLine != undefined)
        {
            this.svg.removeChild(this.crosshairVertLine);
            this.crosshairVertLine = undefined;
        }
        if(this.crosshairHorLine != undefined)
        {
            this.svg.removeChild(this.crosshairHorLine);
            this.crosshairHorLine = undefined;
        }
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        if(this.metrics == undefined)
            this.metrics = new Metrics(METRICS_BORDER, width, height, 0, 1, 0, 1);
        else
            this.metrics = new Metrics(METRICS_BORDER, width, height, this.metrics.xmin, 
                this.metrics.xmax, this.metrics.ymin, this.metrics.ymax);
        this.wave = new Wave(this.signals, this.colors, this.g, this.metrics, this.clippath);
        this.grid = new Grid(this.g, this.metrics, this.labels, this.xIsDateTime);
        this.plot();
    }
    this.resize = resize;
    
    function notifyMode(inMode)
    {
        if(inMode != CROSSHAIR)
        {
           if(this.crosshairVertLine != undefined)
           {
                this.svg.removeChild(this.crosshairVertLine);
                this.crosshairVertLine = undefined;
           }
           if(this.crosshairHorLine != undefined)
           {
                this.svg.removeChild(this.crosshairHorLine);
                this.crosshairHorLine = undefined;
           }
        }
        if(inMode == PAN && this.zoomCache.length > 0)
            this.zoomCache = new Array();
    }
    this.notifyMode = notifyMode;
    
    function startCrosshair(x,y)
    {
        this.crosshairing = true;
        var xVal = this.metrics.getXValue(x);
        var yVal = this.metrics.getYValue(y);
        this.crosshairIdx = this.wave.getCrosshairIdx(xVal, yVal);
        this.crosshairColor = this.wave.getCrosshairColor(this.crosshairIdx);
        var crossY = this.wave.getCrosshairY(xVal, this.crosshairIdx);
        var yPixel = this.metrics.getYPixel(crossY);
        if(this.crosshairVertLine == undefined)
            this.crosshairVertLine = document.createElementNS("http://www.w3.org/2000/svg","line");
        this.crosshairVertLine.setAttribute("x1", x);
        this.crosshairVertLine.setAttribute("x2", x);
        this.crosshairVertLine.setAttribute("y1", 0);
        this.crosshairVertLine.setAttribute("y2", this.svg.viewBox.baseVal.height);
        this.crosshairVertLine.setAttribute("stroke", this.crosshairColor);
        this.crosshairVertLine.setAttribute("stroke-width","1");
        this.svg.appendChild(this.crosshairVertLine);
        if(this.crosshairHorLine == undefined)
            this.crosshairHorLine = document.createElementNS("http://www.w3.org/2000/svg","line");
        this.crosshairHorLine.setAttribute("x1", 0);
        this.crosshairHorLine.setAttribute("x2", this.svg.viewBox.baseVal.width);
        this.crosshairHorLine.setAttribute("y1", yPixel);
        this.crosshairHorLine.setAttribute("y2", yPixel);
        this.crosshairHorLine.setAttribute("stroke",this.crosshairColor);
        this.crosshairHorLine.setAttribute("stroke-width","1");
        this.svg.appendChild(this.crosshairHorLine);
        this.svg.style.cursor = "crosshair";
    }
    this.startCrosshair = startCrosshair;
    
    function dragCrosshair(x,y)
    {
        if(this.crosshairing == undefined || this.crosshairing == false) 
            return;
        var xVal = this.metrics.getXValue(x);
        var yVal = this.wave.getCrosshairY(xVal, this.crosshairIdx);
        var crossY = this.wave.getCrosshairY(xVal, this.crosshairIdx);
        var yPixel = this.metrics.getYPixel(crossY);
        this.crosshairVertLine.setAttribute("x1", x);
        this.crosshairVertLine.setAttribute("x2", x);
        this.crosshairVertLine.setAttribute("y1", 0);
        this.crosshairVertLine.setAttribute("y2", this.svg.viewBox.baseVal.height);
        this.crosshairHorLine.setAttribute("x1", 0);
        this.crosshairHorLine.setAttribute("x2", this.svg.viewBox.baseVal.width);
        this.crosshairHorLine.setAttribute("y1", yPixel);
        this.crosshairHorLine.setAttribute("y2", yPixel);
        document.getElementById("XValue").innerHTML = 'X:'+(Math.round(xVal * 100000)/100000);
        document.getElementById("YValue").innerHTML ='Y:'+Math.round(yVal * 100000)/100000;
        document.getElementById("Expr").innerHTML ='Expr:'+this.wave.getCrosshairExpr(this.crosshairIdx);
        document.getElementById("Shot").innerHTML ='Shot:'+this.wave.getCrosshairShot(this.crosshairIdx);
        dragOtherCrosshairs(this, xVal, yVal);  
    }
    this.dragCrosshair = dragCrosshair;

    //Called when crosshair is dragged in another panel
    function dragCrosshairExternal(xVal, yVal)
    {
        if(this.crosshairing)
            return;
        var xPixel = this.metrics.getXPixel(xVal);
        var yPixel = this.metrics.getYPixel(yVal);
        this.crosshairColor = this.wave.getCrosshairColor(0);
        if(this.crosshairVertLine == undefined)
        {
            this.crosshairVertLine = document.createElementNS("http://www.w3.org/2000/svg","line");
            this.crosshairVertLine.setAttribute("x1", xPixel);
            this.crosshairVertLine.setAttribute("x2", xPixel);
            this.crosshairVertLine.setAttribute("y1", 0);
            this.crosshairVertLine.setAttribute("y2", this.svg.viewBox.baseVal.height);
            this.crosshairVertLine.setAttribute("stroke", this.crosshairColor);
            this.crosshairVertLine.setAttribute("stroke-width","1");
            this.svg.appendChild(this.crosshairVertLine);
        }
        else
        {
            this.crosshairVertLine.setAttribute("x1", xPixel);
            this.crosshairVertLine.setAttribute("x2", xPixel);
            this.crosshairVertLine.setAttribute("y1", 0);
            this.crosshairVertLine.setAttribute("y2", this.svg.viewBox.baseVal.height);
        }
        if(this.crosshairHorLine == undefined)
        {
            this.crosshairHorLine = document.createElementNS("http://www.w3.org/2000/svg","line");
            this.crosshairHorLine.setAttribute("x1", 0);
            this.crosshairHorLine.setAttribute("x2", this.svg.viewBox.baseVal.width);
            this.crosshairHorLine.setAttribute("y1", yPixel);
            this.crosshairHorLine.setAttribute("y2", yPixel);
            this.crosshairHorLine.setAttribute("stroke",this.crosshairColor);
            this.crosshairHorLine.setAttribute("stroke-width","1");
            this.svg.appendChild(this.crosshairHorLine);
        }
        else
        {
            this.crosshairHorLine.setAttribute("x1", 0);
            this.crosshairHorLine.setAttribute("x2", this.svg.viewBox.baseVal.width);
            this.crosshairHorLine.setAttribute("y1", yPixel);
            this.crosshairHorLine.setAttribute("y2", yPixel);
        }
    }
    this.dragCrosshairExternal = dragCrosshairExternal;
    
    function endCrosshair()
    {
        this.svg.style.cursor = "default";
        this.crosshairing = false;
    }
    this.endCrosshair = endCrosshair;
    
    function startPan(x,y)
    {
        this.startPanXVal = this.metrics.getXValue(x);
        this.startPanYVal = this.metrics.getYValue(y);
        this.originalPanMetrics = this.metrics;
        this.svg.style.cursor = "move";
        this.panning = true;
    }
    this.startPan = startPan;

    function dragPan(x, y)
    {
        if(this.panning == undefined || this.panning == false) return;
        var currXVal = this.originalPanMetrics.getXValue(x);
        var currYVal = this.originalPanMetrics.getYValue(y);
        var deltaX = this.startPanXVal - currXVal;
        var deltaY = this.startPanYVal - currYVal;
        this.metrics = new Metrics(METRICS_BORDER, this.originalPanMetrics.width, this.originalPanMetrics.height, 
          this.originalPanMetrics.xmin + deltaX, this.originalPanMetrics.xmax + deltaX, 
          this.originalPanMetrics.ymin + deltaY,  this.originalPanMetrics.ymax + deltaY);
        this.svg.removeChild(this.g);
        this.zoomRect = undefined;
        this.crosshairVertLine = undefined;
        this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.wave = new Wave(this.signals, 0, this.g, this.metrics, this.clippath);
        this.grid = new Grid(this.g, this.metrics, this.labels, this.xIsDateTime);

    // Check if the user is getting close to one end of the current wave and update it accordingly
        var needUpdateThreshold = 0.25;
        var displayedWidth = this.metrics.xmax - this.metrics.xmin;
        if((this.metrics.xmin < this.currentWaveMinX + needUpdateThreshold * displayedWidth
            || this.metrics.xmax > this.currentWaveMaxX - needUpdateThreshold * displayedWidth)
            && this.metrics.xmin > this.xMin && this.metrics.xmax < this.xMax)
        {
            if((this.metrics.xmin < this.currentWaveMinX || this.metrics.xmax > this.currentWaveMaxX) && !this.updating)
            {
                this.signals = this.originalSignals; // Fall back to the original, low-resolution signals in the meantime
                this.wave = new Wave(this.signals, 0, this.g, this.metrics, this.clippath);
            }
            this.plot();
            if(!this.updating)
            {
                // Update the wave taking into consideration the direction (Math.sign(deltaX)) in which the user is moving
                var bufferingFactor = 2;
                this.update(this.metrics.xmin - ((bufferingFactor-1)/2 - needUpdateThreshold*Math.sign(deltaX)) * displayedWidth,
                            this.metrics.xmax + ((bufferingFactor-1)/2 + needUpdateThreshold*Math.sign(deltaX)) * displayedWidth, 
                            bufferingFactor * this.DEFAULT_NUM_SAMPLES);
            }

        }
        else
            this.plot();
    }
    this.dragPan = dragPan;
    
    function endPan()
    {
        this.svg.style.cursor = "default";
        this.panning = false;
    }
    this.endPan = endPan;
    
    function startZoom(x,y)
    {
        this.zoomState = this.STARTING_ZOOM;
        if(this.crosshairVertLine != undefined)
        {
            this.svg.removeChild(this.crosshairVertLine);
            this.crosshairVertLine = undefined;
        }
        if(this.crosshairHorLine != undefined)
        {
            this.svg.removeChild(this.crosshairHorLine);
            this.crosshairHorLine = undefined;
        }
    }
    this.startZoom = startZoom;

    function beginZoom(x, y)
    {
        this.svg.style.cursor = "crosshair";
        if(this.zoomRect)
            this.svg.removeChild(this.zoomRect);
        this.zoomRect = undefined;
        this.zoomX = x;
        this.zoomY = y;
        var zoomRect = document.createElementNS("http://www.w3.org/2000/svg","rect");
        zoomRect.setAttribute("x",x);
        zoomRect.setAttribute("y",y);
        zoomRect.setAttribute("width","1");
        zoomRect.setAttribute("height","1");
        zoomRect.setAttribute("fill","none");
        zoomRect.setAttribute("stroke","black");
        zoomRect.setAttribute("stroke-width","1");
        this.zoomRect = zoomRect;
        this.svg.appendChild(zoomRect);
    }
    this.beginZoom = beginZoom;

    function dragZoom(x, y)
    {
        if(this.zoomState == this.NOT_ZOOMING)
            return;
        if(this.zoomState == this.STARTING_ZOOM)
        {
            this.zoomState = this.ZOOMING;
            this.beginZoom(x, y);
        }
        var actWidth, actHeight, actX, actY;
        var currWidth = x - this.zoomX;
        var currHeight = y - this.zoomY;
        if(this.zoomRect)
        {
            if(currWidth > 0)
            {
                actX = this.zoomX;
                actWidth = currWidth;
            }
            else
            {
                actX = x;
                actWidth = -currWidth;
            }
            if(currHeight > 0)
            {
                actY = this.zoomY;
                actHeight = currHeight;
            }
            else
            {
                actY = y;
                actHeight = -currHeight;
            }
            this.zoomRect.setAttribute("x",actX);
            this.zoomRect.setAttribute("y",actY);
            this.zoomRect.setAttribute("width",actWidth);
            this.zoomRect.setAttribute("height",actHeight);
        }
    }
    this.dragZoom = dragZoom;

    function isZooming()
    {
        return (this.zoomState == this.ZOOMING);
    }
    this.isZooming = isZooming;

    function endZoom(x, y)
    {
        if(this.zoomState != this.ZOOMING)
        {
            this.zoomState = this.NOT_ZOOMING;
            return;
        }
        this.svg.style.cursor = "default";
        if(this.zoomRect)
            this.svg.removeChild(this.zoomRect);
        this.zoomRect = undefined;
        //Check for 'fake' zooms (i.e. selecting an area which smaller in width or height tham 1/100 of the window size)'
        if(Math.abs(x - this.zoomX) < this.metrics.width/100 || Math.abs(y - this.zoomY) < this.metrics.height/100)
        {
            this.zoomState = this.NOT_ZOOMING;
            return;
        }
        this.zoomCache.push(this.wave);
        var xMin, xMax, yMin, yMax;
        if(x < this.zoomX)
        {
            xMin = this.metrics.getXValue(x);
            xMax = this.metrics.getXValue(this.zoomX);
        }
        else
        {
            xMin = this.metrics.getXValue(this.zoomX);
            xMax = this.metrics.getXValue(x);
        }
        if(y > this.zoomY)
        {
            yMin = this.metrics.getYValue(y);
            yMax = this.metrics.getYValue(this.zoomY);
        }
        else
        {
            yMin = this.metrics.getYValue(this.zoomY);
            yMax = this.metrics.getYValue(y);
        }
        this.svg.removeChild(this.g);
        this.zoomRect = undefined;
        this.crosshairVertLine = undefined;
        this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(METRICS_BORDER, this.metrics.width, this.metrics.height, xMin, xMax, yMin, yMax);
        this.wave = new Wave(this.signals, 0, this.g, this.metrics, this.clippath);
        this.grid = new Grid(this.g, this.metrics, this.labels, this.xIsDateTime);
        this.plot();
        this.update();
        this.zoomState = this.NOT_ZOOMING;
    }
    this.endZoom = endZoom;

    function autoscale()
    {
        this.svg.removeChild(this.g);
        this.zoomRect = undefined;
        if(this.crosshairVertLine != undefined)
        {
            this.svg.removeChild(this.crosshairVertLine);
            this.crosshairVertLine = undefined;
        }
        if(this.crosshairHorLine != undefined)
        {
            this.svg.removeChild(this.crosshairHorLine);
            this.crosshairHorLine = undefined;
        }
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.signals = this.originalSignals;
        this.metrics = new Metrics(METRICS_BORDER, this.metrics.width, this.metrics.height, this.xMin, 
            this.xMax, this.yMin, this.yMax);
        this.wave = new Wave(this.signals, 0, this.g, this.metrics, this.clippath);
        this.grid = new Grid(this.g, this.metrics, this.labels, this.xIsDateTime);
        this.plot();
    }
    this.autoscale = autoscale;
    function getMinYInRange(x1, x2)
    {
        var ymin = this.signals[0].getMinYInRange(x1, x2);
        for(var sigIdx = 1; sigIdx < this.signals.length; sigIdx++)
        {
            var currMin = this.signals[sigIdx].getMinYInRange(x1, x2);
            if(currMin < ymin)
                ymin = currMin;
        }
        return ymin;
    }
    this.getMinYInRange = getMinYInRange;
    function getMaxYInRange(x1, x2)
    {
        var ymax = this.signals[0].getMaxYInRange(x1, x2);
        for(var sigIdx = 1; sigIdx < this.signals.length; sigIdx++)
        {
            var currMax = this.signals[sigIdx].getMaxYInRange(x1, x2);
            if(currMax > ymax)
                ymax = currMax;
        }
        return ymax;
    }
    this.getMaxYInRange = getMaxYInRange;
    
    function autoscaleY()
    {
        this.svg.removeChild(this.g);
        this.zoomRect = undefined;
        if(this.crosshairVertLine != undefined)
        {
            this.svg.removeChild(this.crosshairVertLine);
            this.crosshairVertLine = undefined;
        }
        if(this.crosshairHorLine != undefined)
        {
            this.svg.removeChild(this.crosshairHorLine);
            this.crosshairHorLine = undefined;
        }
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(METRICS_BORDER, this.metrics.width, this.metrics.height, this.metrics.xmin, 
            this.metrics.xmax, this.getMinYInRange(this.metrics.xmin,this.metrics.xmax),
            this.getMaxYInRange(this.metrics.xmin,this.metrics.xmax));
        this.wave = new Wave(this.signals, 0, this.g, this.metrics, this.clippath);
        this.grid = new Grid(this.g, this.metrics, this.labels, this.xIsDateTime);
        this.plot();
    }
    this.autoscaleY = autoscaleY;
    
    function setScale(xmin, xmax, ymin, ymax)
    {
        this.svg.removeChild(this.g);
        this.zoomRect = undefined;
        if(this.crosshairVertLine != undefined)
        {
            this.svg.removeChild(this.crosshairVertLine);
            this.crosshairVertLine = undefined;
        }
        if(this.crosshairHorLine != undefined)
        {
            this.svg.removeChild(this.crosshairHorLine);
            this.crosshairHorLine = undefined;
        }
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(METRICS_BORDER, this.metrics.width, this.metrics.height, xmin, xmax, ymin, ymax);
        this.wave = new Wave(this.signals, 0, this.g, this.metrics, this.clippath);
        this.grid = new Grid(this.g, this.metrics, this.labels, this.xIsDateTime);
        this.plot();
        this.update();
    }
    this.setScale = setScale;
    function setScaleAutoY(xmin, xmax)
    {
        this.svg.removeChild(this.g);
        this.zoomRect = undefined;
        if(this.crosshairVertLine != undefined)
        {
            this.svg.removeChild(this.crosshairVertLine);
            this.crosshairVertLine = undefined;
        }
        if(this.crosshairHorLine != undefined)
        {
            this.svg.removeChild(this.crosshairHorLine);
            this.crosshairHorLine = undefined;
        }
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(METRICS_BORDER, this.metrics.width, this.metrics.height, xmin, xmax, 
             this.getMinYInRange(xmin,xmax),
             this.getMaxYInRange(xmin,xmax));
        this.wave = new Wave(this.signals, 0, this.g, this.metrics, this.clippath);
        this.grid = new Grid(this.g, this.metrics, this.labels, this.xIsDateTime);
        this.plot();
        this.update();
    }
    this.setScaleAutoY = setScaleAutoY;
    
    function resetScales()
    {
        this.svg.removeChild(this.g);
        this.zoomRect = undefined;
        if(this.zoomCache.length > 0)
            this.zoomCache = new Array();
        if(this.crosshairVertLine != undefined)
        {
            this.svg.removeChild(this.crosshairVertLine);
            this.crosshairVertLine = undefined;
        }
        if(this.crosshairHorLine != undefined)
        {
            this.svg.removeChild(this.crosshairHorLine);
            this.crosshairHorLine = undefined;
        }
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        var xmin, xmax, ymin, ymax;
        if(this.limits.xmin != undefined)
            xmin = this.limits.xmin;
        else
            xmin = this.xMin;
        if(this.limits.xmax != undefined)
            xmax = this.limits.xmax;
        else
            xmax = this.xMax;
        if(this.limits.ymin != undefined)
            ymin = this.limits.ymin;
        else
            ymin = this.yMin;
        if(this.limits.ymax != undefined)
            ymax = this.limits.ymax;
        else
            ymax = this.yMax;
        this.metrics = new Metrics(METRICS_BORDER, this.metrics.width, this.metrics.height, xmin, xmax, ymin, ymax);
        this.wave = new Wave(this.signals, 0, this.g, this.metrics, this.clippath);
        this.grid = new Grid(this.g, this.metrics, this.labels, this.xIsDateTime);
        this.plot();
        this.update();
    }
    this.resetScales = resetScales;

    function undoZoom()
    {
        this.svg.removeChild(this.g);
        this.zoomRect = undefined;
        if(this.crosshairVertLine != undefined)
        {
            this.svg.removeChild(this.crosshairVertLine);
            this.crosshairVertLine = undefined;
        }
        if(this.crosshairHorLine != undefined)
        {
            this.svg.removeChild(this.crosshairHorLine);
            this.crosshairHorLine = undefined;
        }
        this.wave = this.zoomCache.pop();
        this.g = this.wave.g;
        this.svg.appendChild(this.g);
        this.signals = this.wave.signals;
        this.metrics = this.wave.metrics;
        this.grid = new Grid(this.g, this.metrics, this.labels, this.xIsDateTime);
        this.plot();
    }
    this.undoZoom = undoZoom;

}

function FramePanel(svg, canvas, numCols, numRows, col, row, tree, shots, signalExprs, frameSettings, labelExprs)
{
    this.svg = svg;
    this.canvas = canvas;
    this.ctx = canvas.getContext('2d');

    this.numCols = numCols;
    this.numRows = numRows;
    this.col = col;
    this.row = row;
    this.tree = tree;
    this.shots = shots;
    this.signalExpr = signalExprs[0];
    this.frameSettings = frameSettings;
    this.labelExprs = labelExprs;
    this.currentFrameIdx = 0;
    this.updating = false;

    function update(frameIdx, isInit)
    {
        this.updating = true;
        if(frameIdx == undefined)
            frameIdx = 0;
        if(isInit == undefined)
            isInit = true;
        var req = new XMLHttpRequest();
        var getStr = 'frame?y=' + this.signalExpr + '&frame_idx=' + frameIdx;
        if (this.tree != undefined)
        {
            getStr = getStr + '&tree=' + this.tree;
            if(this.shots != undefined)
                getStr = getStr + '&shot=' + this.shots[0];
        }
        if(this.labelExprs.title != undefined)
             getStr = getStr + '&title=' + labelExprs.title;
        if(this.labelExprs.xLabel != undefined)
             getStr = getStr + '&xlabel=' + labelExprs.xLabel;
        if(this.labelExprs.yLabel != undefined)
             getStr = getStr + '&ylabel=' + labelExprs.yLabel;
        if(isInit)
            getStr = getStr + '&init=true';
        req.open('GET', getStr, true);
        req.framePanel = this;
        req.frameIdx = frameIdx;
        req.isInit = isInit;
        req.onreadystatechange = function() {
            if (req.readyState == 4)
            {
                if (this.getResponseHeader('ERROR'))
                    this.framePanel.updateFailure(this.getResponseHeader('ERROR'));
                else
                {
                    var frameWidth = this.getResponseHeader('FRAME_WIDTH');
                    var frameHeight = this.getResponseHeader('FRAME_HEIGHT');
                    var frameBitsPerPixel = 8 * parseInt(this.getResponseHeader('FRAME_BYTES_PER_PIXEL'));
                    var frameLength = parseInt(this.getResponseHeader('FRAME_LENGTH'));
//                    var frameData = eval('new Uint' +  frameBitsPerPixel + 'Array(this.response, 0, frameLength)');
                    var frameData = eval('new Uint' +  frameBitsPerPixel + 'Array(this.response, 0, frameWidth*frameHeight)');
                    if(isInit)
                    {
                        var timesDataType = this.getResponseHeader('TIMES_DATATYPE');
                        var timesLength = parseInt(this.getResponseHeader('TIMES_LENGTH'));
                        try {
//                             var tempBuf = eval('new Int8Array(this.response, frameLength * frameWidth * frameHeight *frameData.BYTES_PER_ELEMENT, timesLength*8)');
//                             this.framePanel.times = new Float64Array(tempBuf);
                             this.framePanel.times = eval('new ' + timesDataType + '(this.response, frameLength * frameWidth*frameHeight*frameData.BYTES_PER_ELEMENT, timesLength)');
                        }catch(err) {alert(err);}
                    }
                    this.framePanel.labels = new Object();
                    if(this.framePanel.labelExprs.title != undefined)
                        this.framePanel.labels.title = this.getResponseHeader('title');
                    if(this.framePanel.labelExprs.xLabel != undefined)
                        this.framePanel.labels.xlabel = this.getResponseHeader('xlabel');
                    if(this.framePanel.labelExprs.yLabel != undefined)
                        this.framePanel.labels.ylabel = this.getResponseHeader('ylabel');
                    this.framePanel.updateSuccess(frameData, frameWidth, frameHeight, this.frameIdx, this.isInit);
                }
            }
        };
        req.responseType = 'arraybuffer';
        req.send();
    }
    this.update = update;

    function updateSuccess(frameData, frameWidth, frameHeight, frameIdx, isInit)
    {
        var imgData = this.ctx.createImageData(frameWidth, frameHeight);
        var colorTable = colorTables[this.frameSettings.palette];
        if(colorTable == undefined)
            colorTable = colorTables['B-W LINEAR'];
        var bitShift = this.frameSettings.bitShift;
        if(frameData.BYTES_PER_ELEMENT == 1)
            bitShift = 0;
        for (var i = 0, j = 0, len = frameWidth * frameHeight; j < len; i += 4, j++)
        {
            var val;
            if(bitShift > 0)
                val = frameData[j] << bitShift;
            else
                val = frameData[j] >> -bitShift;
            if(val > 255)
            {
                if(this.frameSettings.bitClip)
                    val = 255;
                else
                    val = 0;
            }
            imgData.data[i]     = colorTable.r[val];  // red
            imgData.data[i + 1] = colorTable.g[val];  // green
            imgData.data[i + 2] = colorTable.b[val];  // blue
            imgData.data[i + 3] = 255;                // alpha
        }
        this.originalCanvas = document.createElement('canvas');
        this.originalCtx = this.originalCanvas.getContext('2d');
        this.originalCanvas.width = frameWidth;
        this.originalCanvas.height = frameHeight;
        
        this.originalCtx.putImageData(imgData, 0, 0);

        if(isInit)
        {
            this.pixelMinX = 0;
            this.pixelMaxX = this.originalCanvas.width;
            this.pixelMinY = 0;
            this.pixelMaxY = this.originalCanvas.height;
            this.canvas.width = frameWidth;
            this.canvas.height = frameHeight;
        }
        this.drawFrame(parseFloat(this.canvas.parentNode.width), parseFloat(this.canvas.parentNode.height));
        this.currentFrameIdx = frameIdx;
        this.setLabels();
        
        this.updating = false;

        if(this.nextFrameTime != undefined)
        {
            var nextFrameIdx = this.timeToFrameIdx(this.nextFrameTime);
            if(nextFrameIdx != undefined)
            {
                if(nextFrameIdx != this.currentFrameIdx)
                    this.update(nextFrameIdx, false);   
            }
            else
            {
                this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
                this.errorMessage = 'No frame at time ' + this.nextFrameTime.toFixed(3);
                this.setLabels();
                this.errorMessage = undefined;
            }
            this.nextFrameTime = undefined;
        }
    }
    this.updateSuccess = updateSuccess;

    function updateFailure(error)
    {
        alert('Error getting signals for panel ('+this.row+', '+this.col+'): '+error);
        this.updating = false;
    }
    this.updateFailure = updateFailure;

    function setLabels()
    {
        if(this.labelsGroup != undefined)
            this.svg.removeChild(this.labelsGroup);
        this.labelsGroup = document.createElementNS('http://www.w3.org/2000/svg','g');
        this.svg.appendChild(this.labelsGroup);
        if(this.labels.title != undefined || this.errorMessage != undefined)
        {
            var title = document.createElementNS('http://www.w3.org/2000/svg','text');
            title.setAttributeNS(null,'text-anchor','middle');
            title.setAttributeNS(null,'x',this.svg.viewBox.baseVal.width/2);
            title.setAttributeNS(null,'y',18);
            title.setAttributeNS(null,'font-size','14px');
            title.setAttributeNS(null,'fill','white');
            this.labelsGroup.appendChild(title);
            if(this.labels.title != undefined)
            {
                var titleLine = document.createElementNS('http://www.w3.org/2000/svg','tspan');
                titleLine.setAttributeNS(null,'x',this.svg.viewBox.baseVal.width/2);
                titleLine.appendChild(document.createTextNode(this.labels.title));
                title.appendChild(titleLine);
            }
            if(this.errorMessage != undefined)
            {
                var errorLine = document.createElementNS('http://www.w3.org/2000/svg','tspan');
                errorLine.setAttributeNS(null,'x',this.svg.viewBox.baseVal.width/2);
                errorLine.setAttributeNS(null,'y',38);
                errorLine.appendChild(document.createTextNode(this.errorMessage));
                title.appendChild(errorLine);
            }
        }        
        if(this.labels.ylabel != undefined)
        {
            var ylabel = document.createElementNS('http://www.w3.org/2000/svg','text');
            this.labelsGroup.appendChild(ylabel);
            ylabel.setAttributeNS(null,'text-anchor','middle');
            ylabel.setAttributeNS(null,'y',this.svg.viewBox.baseVal.height/2);
            ylabel.setAttributeNS(null,'x',0);
            ylabel.setAttribute('transform','rotate(270,'+16+','+this.svg.viewBox.baseVal.height/2+')'), 
            ylabel.setAttributeNS(null,'font-size','16px');
            ylabel.setAttributeNS(null,'fill','white');
            ylabel.appendChild(document.createTextNode(this.labels.ylabel));
        }
        if(this.labels.xlabel != undefined)
        {
            var xlabel = document.createElementNS('http://www.w3.org/2000/svg','text');
            this.labelsGroup.appendChild(xlabel);
            xlabel.setAttributeNS(null,'text-anchor','middle');
            xlabel.setAttributeNS(null,'y',this.svg.viewBox.baseVal.height - 5);
            xlabel.setAttributeNS(null,'x',this.svg.viewBox.baseVal.width/2);
            xlabel.setAttributeNS(null,'font-size','16px');
            xlabel.setAttributeNS(null,'fill','white');
            xlabel.appendChild(document.createTextNode(this.labels.xlabel));
        }
    }
    this.setLabels = setLabels;

    function startPlay()
    {      
        var framePanel = this;
        this.playInterval = setInterval(function() {
            framePanel.currentFrameIdx = (framePanel.currentFrameIdx + 1) % framePanel.times.length;
            framePanel.update(framePanel.currentFrameIdx, false);
            if(mode == CROSSHAIR)
            {
                for(var idx = 0; idx < wavePanels.length; idx++)
                    if(wavePanels[idx] != framePanel)
                        wavePanels[idx].dragCrosshairExternal(framePanel.times[framePanel.currentFrameIdx], 0);
            }
        }, 1000);
        this.playing = true;
    }
    this.startPlay = startPlay;

    function stopPlay()
    {
        clearInterval(this.playInterval);
        this.playing = false;
    }
    this.stopPlay = stopPlay;

    //Called when crosshair is dragged in another panel
    function dragCrosshairExternal(xVal, yVal)
    {
        this.nextFrameTime = xVal;
        if(!this.updating)
        {
            var frameIdx = this.timeToFrameIdx(xVal);
            if(frameIdx != undefined)
                this.update(frameIdx, false);
            else
            {
                this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
                this.errorMessage = 'No frame at time ' + xVal.toFixed(3);
                this.setLabels();
                this.errorMessage = undefined;                
            }
        }
    }
    this.dragCrosshairExternal = dragCrosshairExternal;

    function timeToFrameIdx(time)
    {
        if(time < this.times[0])
            return undefined;
        for (var idx = 0; idx < this.times.length - 1; idx++)
        {
            if(time >= this.times[idx] && time < this.times[idx + 1])
                return idx;
        }
        return undefined;
    }
    this.timeToFrameIdx = timeToFrameIdx;

    function drawFrame(width, height)
    {
        if(this.frameSettings.keepRatio)
        {
            var frameAspectRatio = this.canvas.width / this.canvas.height;
            if(width/height > frameAspectRatio)
            {
                this.canvas.setAttribute('width', height * frameAspectRatio);
                this.canvas.setAttribute('height', height);
            }
            else
            {
                this.canvas.setAttribute('width', width);
                this.canvas.setAttribute('height', width / frameAspectRatio);
            }
        }
        else
        {
            this.canvas.setAttribute('width', width);
            this.canvas.setAttribute('height', height);
        }

        var scaleH = 1, scaleV = 1;
        var translateH = 0, translateV = 0;
        if(this.frameSettings.horizontalFlip)
        {
            scaleH = -1;
            translateH = -this.canvas.width;
        }
        if(this.frameSettings.verticalFlip)
        {
            scaleV = -1;
            translateV = -this.canvas.height;
        }
        this.ctx.scale(scaleH, scaleV);
        this.ctx.translate(translateH, translateV);

        this.ctx.imageSmoothingEnabled = false;
        this.ctx.mozImageSmoothingEnabled = false;
        this.ctx.msImageSmoothingEnabled = false;
        this.ctx.drawImage(this.originalCanvas, this.pixelMinX, this.pixelMinY, this.pixelMaxX - this.pixelMinX, this.pixelMaxY - this.pixelMinY, 0, 0, this.canvas.width, this.canvas.height);
        this.svg.setAttributeNS(null, 'width', width);
        this.svg.setAttributeNS(null, 'height', height);
        this.svg.setAttribute('viewBox', '0 0 ' + width + ' ' + height);
        this.svg.parentNode.setAttribute('width', width);
        this.svg.parentNode.setAttribute('height', height);
    }
    this.drawFrame = drawFrame;

    function resize(width, height)
    {
        this.drawFrame(width, height);
    }
    this.resize = resize;

    function enableMouseMove()
    {
        this.svg.addEventListener('mousemove', this.mouseMove = function(evt) {
            mouseMove(evt);
        }, false);
    }
    this.enableMouseMove = enableMouseMove;
    
    function disableMouseMove()
    {
        this.svg.removeEventListener('mousemove', this.mouseMove, false);
    }
    this.disableMouseMove = disableMouseMove;

    function notifyMode(inMode)
    {
        if(inMode != CROSSHAIR)
        {
            if(this.crosshairVertLine != undefined)
            {
                this.svg.removeChild(this.crosshairVertLine);   
                this.crosshairVertLine = undefined;
            }
            if(this.crosshairHorLine != undefined)
            {
                this.svg.removeChild(this.crosshairHorLine);   
                this.crosshairHorLine = undefined;
            }
        }
    }
    this.notifyMode = notifyMode;

    function startCrosshair(x, y)
    {
        this.crosshairing = true;
        if(this.crosshairVertLine == undefined)
            this.crosshairVertLine = document.createElementNS('http://www.w3.org/2000/svg','line');
        this.crosshairVertLine.setAttribute('x1', x);
        this.crosshairVertLine.setAttribute('x2', x);
        this.crosshairVertLine.setAttribute('y1', 0);
        this.crosshairVertLine.setAttribute('y2', this.svg.viewBox.baseVal.height);
        this.crosshairVertLine.setAttribute('stroke', 'white');
        this.crosshairVertLine.setAttribute('stroke-width','1');
        this.svg.appendChild(this.crosshairVertLine);
        if(this.crosshairHorLine == undefined)
            this.crosshairHorLine = document.createElementNS('http://www.w3.org/2000/svg','line');
        this.crosshairHorLine.setAttribute('x1', 0);
        this.crosshairHorLine.setAttribute('x2', this.svg.viewBox.baseVal.width);
        this.crosshairHorLine.setAttribute('y1', y);
        this.crosshairHorLine.setAttribute('y2', y);
        this.crosshairHorLine.setAttribute('stroke', 'white');
        this.crosshairHorLine.setAttribute('stroke-width','1');
        this.svg.appendChild(this.crosshairHorLine);
        this.svg.style.cursor = 'crosshair';
    }
    this.startCrosshair = startCrosshair;
    
    function dragCrosshair(x, y)
    {
        if(this.crosshairing == undefined || this.crosshairing == false) 
            return;
        this.crosshairVertLine.setAttribute('x1', x);
        this.crosshairVertLine.setAttribute('x2', x);
        this.crosshairVertLine.setAttribute('y1', 0);
        this.crosshairVertLine.setAttribute('y2', this.svg.viewBox.baseVal.height);
        this.crosshairHorLine.setAttribute('x1', 0);
        this.crosshairHorLine.setAttribute('x2', this.svg.viewBox.baseVal.width);
        this.crosshairHorLine.setAttribute('y1', y);
        this.crosshairHorLine.setAttribute('y2', y);
        document.getElementById('XValue').innerHTML = 'X:' + x;
        document.getElementById('YValue').innerHTML = 'Y:' + y;
        document.getElementById('Expr').innerHTML = 'Expr:' + this.signalExpr;
        document.getElementById('Shot').innerHTML = 'Shot:' + this.shots[0];
    }
    this.dragCrosshair = dragCrosshair;
    
    function endCrosshair()
    {
        this.svg.style.cursor = 'default';
        this.crosshairing = false;
    }
    this.endCrosshair = endCrosshair;

    function startZoom(x, y)
    {
        if(x > this.canvas.width || y > this.canvas.height)
            return;
        this.zooming = true;
        if(this.zoomRect != undefined)
            this.svg.removeChild(this.zoomRect);
        this.zoomRect = undefined;
        this.startZoomX = x;
        this.startZoomY = y;
        var zoomRect = document.createElementNS('http://www.w3.org/2000/svg','rect');
        zoomRect.setAttribute('x',x);
        zoomRect.setAttribute('y',y);
        zoomRect.setAttribute('width','1');
        zoomRect.setAttribute('height','1');
        zoomRect.setAttribute('fill','none');
        zoomRect.setAttribute('stroke','white');
        zoomRect.setAttribute('stroke-width','1');
        this.zoomRect = zoomRect;
        this.svg.appendChild(zoomRect);
        this.svg.style.cursor = 'crosshair';
    }
    this.startZoom = startZoom;

    function dragZoom(x, y)
    {
        if(!this.zooming)
            return;
        if(x > this.canvas.width)
            x = this.canvas.width;
        if(y > this.canvas.height)
            y = this.canvas.height;
        var actWidth, actHeight, actX, actY;
        var currWidth = x - this.startZoomX;
        var currHeight = y - this.startZoomY;
        if(this.zoomRect != undefined)
        {
            if(currWidth > 0)
            {
                actX = this.startZoomX;
                actWidth = currWidth;
            }
            else
            {
                actX = x;
                actWidth = -currWidth;
            }
            if(currHeight > 0)
            {
                actY = this.startZoomY;
                actHeight = currHeight;
            }
            else
            {
                actY = y;
                actHeight = -currHeight;
            }
            this.zoomRect.setAttribute('x',actX);
            this.zoomRect.setAttribute('y',actY);
            this.zoomRect.setAttribute('width',actWidth);
            this.zoomRect.setAttribute('height',actHeight);
        }
        document.getElementById('XValue').innerHTML = 'X:' + x;
        document.getElementById('YValue').innerHTML = 'Y:' + y;
        document.getElementById('Expr').innerHTML = 'Expr:' + this.signalExpr;
        document.getElementById('Shot').innerHTML = 'Shot:' + this.shots[0];
    }
    this.dragZoom = dragZoom;

    function endZoom(x, y)
    {
        this.zooming = false;
        this.svg.style.cursor = 'default';
        if(this.zoomRect != undefined)
            this.svg.removeChild(this.zoomRect);
        this.zoomRect = undefined;
        //Check for 'fake' zooms (i.e. selecting an area which smaller in width or height tham 1/100 of the window size)'
        if(Math.abs(x - this.startZoomX) < this.svg.viewBox.baseVal.width/100 || Math.abs(y - this.startZoomY) < this.svg.viewBox.baseVal.height/100)
            return;

        this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
        if(this.zoomRatioX == undefined)
        {
            this.zoomRatioX = 1;
            this.zoomRatioY = 1;
        }
        if(this.frameSettings.horizontalFlip)
        {
            x = this.canvas.width - x;
            this.startZoomX = this.canvas.width - this.startZoomX;
        }
        if(this.frameSettings.verticalFlip)
        {
            y = this.canvas.height - y;
            this.startZoomY = this.canvas.height - this.startZoomY;
        }

        var x0 = Math.min(x, this.startZoomX);
        var x1 = Math.max(x, this.startZoomX);
        var y0 = Math.min(y, this.startZoomY);
        var y1 = Math.max(y, this.startZoomY);
        var offsetX = this.pixelMinX;
        var offsetY = this.pixelMinY;
        this.pixelMinX = offsetX + (x0 / this.zoomRatioX * (this.originalCanvas.width  / this.canvas.width));
        this.pixelMaxX = offsetX + (x1 / this.zoomRatioX * (this.originalCanvas.width  / this.canvas.width));
        this.pixelMinY = offsetY + (y0 / this.zoomRatioY * (this.originalCanvas.height / this.canvas.height));
        this.pixelMaxY = offsetY + (y1 / this.zoomRatioY * (this.originalCanvas.height / this.canvas.height));
        this.zoomRatioX *= this.canvas.width / (x1 - x0);
        this.zoomRatioY *= this.canvas.height / (y1 - y0);
        this.canvas.width = x1 - x0;
        this.canvas.height = y1 - y0;
        this.drawFrame(parseFloat(this.canvas.parentNode.width), parseFloat(this.canvas.parentNode.height));
    }
    this.endZoom = endZoom;

    function isZooming()
    {
        return this.zooming;
    }
    this.isZooming = isZooming;

    function startPan(x, y)
    {
        this.panning = true;
        this.startPanX = x;
        this.startPanY = y;
        this.startPanRect = new Object();
        this.startPanRect.pixelMinX = this.pixelMinX;
        this.startPanRect.pixelMaxX = this.pixelMaxX;
        this.startPanRect.pixelMinY = this.pixelMinY;
        this.startPanRect.pixelMaxY = this.pixelMaxY;
        this.svg.style.cursor = 'move';
    }
    this.startPan = startPan;

    function dragPan(x, y)
    {
        if(this.panning == undefined || this.panning == false)
            return;
        var deltaX = (this.startPanX - x) / this.zoomRatioX;
        if(this.frameSettings.horizontalFlip)
            deltaX = -deltaX;
        var deltaY = (this.startPanY - y) / this.zoomRatioY;
        if(this.frameSettings.verticalFlip)
            deltaY = -deltaY;
        this.pixelMinX = this.startPanRect.pixelMinX + deltaX;
        this.pixelMaxX = this.startPanRect.pixelMaxX + deltaX;
        this.pixelMinY = this.startPanRect.pixelMinY + deltaY;
        this.pixelMaxY = this.startPanRect.pixelMaxY + deltaY;
        this.drawFrame(parseFloat(this.canvas.parentNode.width), parseFloat(this.canvas.parentNode.height));
    }
    this.dragPan = dragPan;

    function endPan()
    {
        this.svg.style.cursor = 'default';
        this.panning = false;
    }
    this.endPan = endPan;

    function autoscale()
    {
        if(this.crosshairVertLine != undefined)
        {
             this.svg.removeChild(this.crosshairVertLine);   
             this.crosshairVertLine = undefined;
        }
        if(this.crosshairHorLine != undefined)
        {
             this.svg.removeChild(this.crosshairHorLine);   
             this.crosshairHorLine = undefined;
        }
        if(this.zoomRect != undefined)
        {
             this.svg.removeChild(this.zoomRect);   
             this.zoomRect = undefined;
        }
        this.canvas.width = this.originalCanvas.width;
        this.canvas.height = this.originalCanvas.height;
        this.pixelMinX = 0;
        this.pixelMaxX = this.originalCanvas.width;
        this.pixelMinY = 0
        this.pixelMaxY = this.originalCanvas.height;
        this.zoomRatioX = 1;
        this.zoomRatioY = 1;
        this.drawFrame(parseFloat(this.canvas.parentNode.width), parseFloat(this.canvas.parentNode.height));
    }
    this.autoscale = autoscale;

    this.update(0);
}

function resizeWaves(width, height)
{
    for(var idx = 0; idx < wavePanels.length; idx++)
    {
        wavePanels[idx].resize(width/wavePanels[idx].numCols, height/wavePanels[idx].numRows);
    }
}

function resizeScope()
{
    resizeWaves(window.innerWidth-40,window.innerHeight-100);
    document.getElementById('ScopeArea').setAttribute('height', window.innerHeight-100);
}

function eventUpdate(event,response,panel)
{
    panel.originalSignals = undefined;
    panel.metrics = undefined;
    panel.xMin = undefined;
    panel.xMax = undefined;
    panel.yMin = undefined;
    panel.yMax = undefined;
    panel.xIsDateTime = false;
    panel.update();
}

function updateTitle(expression)
{
    var req = new XMLHttpRequest();
    req.url = 'scope?title='+expression;
    req.open('GET',req.url,true);
    req.responseType='text';
    req.onreadystatechange = function() {
        if (this.readyState == 4) {
            if (this.status == 200) document.title=decodeUrl(this.response);
            this.onreadystatechange = function() {};
        }
    };
    req.send();
}
    
function eventUpdateFail(event,response,panel) {}

function mdsplusEvent(event,repeat,success_cb,error_cb,userarg)
{
    var req = new XMLHttpRequest();
    req.url='event/'+event;
    req.open('GET',req.url,true);
    req.responseType = 'text';
    req.scb=success_cb;
    req.fcb=error_cb;
    req.event=event;
    req.userarg=userarg;
    req.repeat=repeat;
    req.onreadystatechange = function() {
        if (this.readyState == 4) {
            if (this.status == 200) {
                this.scb(this.event,this.response,this.userarg);
                if (this.repeat != 0) {
                    this.open('GET',this.url,true);
                    this.send();
                    return;
                }
            }
            else if (req.status == 204) {
                this.open('GET',this.url,true);
                this.send();
                return;
            } else if (req.status != 0) {
                this.fcb(this.event,req.response,this.userarg);
            }
            this.onreadystatechange = function() {}
        }
    };
    req.send();
    return req;
}

function createWavePanel(div, width, height, numCols, numRows, col, row, tree, shot, exprArray, colors, modes, limits, labels, continuousUpdate)
{
    var svg = document.createElementNS("http://www.w3.org/2000/svg","svg");
    svg.setAttribute("version","1.1");
    svg.setAttributeNS(null,"preserveAspectRatio","none");
    svg.setAttribute("baseProfile","full");
    svg.setAttributeNS(null,"width",width);
    svg.setAttributeNS(null,"height",height);
    svg.setAttributeNS(null,"animatable","no");
    svg.setAttribute("viewBox","0 0 "+width +" " +height);
    svg.setAttribute("onmousedown", "return mouseDown(evt)");
    svg.setAttribute("onmouseup", "return mouseUp(evt)");
    svg.setAttribute("ontouchmove", "mouseMove(evt)");
    svg.setAttribute("ontouchup", "mouseUp(evt)");
    svg.addEventListener('touchstart', function(evt) {
        touchStart(evt);
    }, false);
    svg.addEventListener('touchmove', function(evt) {
        touchMove(evt);
    }, false);
    svg.addEventListener('touchend', function(evt) {
        touchEnd(evt);
    }, false);

    var clippath=document.createElementNS("http://www.w3.org/2000/svg","clipPath");
    clippath.setAttribute("id","clippath"+Math.random());
    var path=document.createElementNS("http://www.w3.org/2000/svg","path");
    path.setAttribute("d","M 0 0 0 " + height + " " + width + " " + height + " " + width + " 0");
    clippath.appendChild(path);
    svg.appendChild(clippath);

    div.appendChild(svg);
    div.setAttribute("width", width);
    div.setAttribute("height", height);
    div.className = "WavePanel";

    wavePanel = new WavePanel(svg, wavePanels.length, numCols, numRows, col, row, clippath, tree, shot, colors, modes, exprArray, limits, labels, undefined, continuousUpdate);
    wavePanels.push(wavePanel);
    return wavePanel;
}

function createFramePanel(div, width, height, numCols, numRows, col, row, tree, shot, exprArray, frameSettings, limits, labels)
{
    var svg = document.createElementNS("http://www.w3.org/2000/svg","svg");
    svg.setAttribute("version","1.1");
    svg.setAttributeNS(null,"preserveAspectRatio","none");
    svg.setAttribute("baseProfile","full");
    svg.setAttributeNS(null,"width",width);
    svg.setAttributeNS(null,"height",height);
    svg.setAttributeNS(null,"animatable","no");
    svg.setAttribute("viewBox","0 0 "+width +" " +height);
    svg.setAttribute("onmousedown", "return mouseDown(evt)");
    svg.setAttribute("onmouseup", "return mouseUp(evt)");
    svg.setAttribute("ontouchmove", "mouseMove(evt)");
    svg.setAttribute("ontouchup", "mouseUp(evt)");
    svg.addEventListener('touchstart', function(evt) {
        touchStart(evt);
    }, false);
    svg.addEventListener('touchmove', function(evt) {
        touchMove(evt);
    }, false);
    svg.addEventListener('touchend', function(evt) {
        touchEnd(evt);
    }, false);
    svg.className.baseVal = "FramePanelSvg";

    var canvas = document.createElement("canvas");
    div.appendChild(svg);
    div.appendChild(canvas);
    div.setAttribute("width", width);
    div.setAttribute("height", height);
    div.className = "FramePanel";

    framePanel = new FramePanel(svg, canvas, numCols, numRows, col, row, tree, shot, exprArray, frameSettings, labels);
    wavePanels.push(framePanel);
    return framePanel;
}

function updateGlobalShots()
{
    var shotList = document.getElementById('GlobalShots').value;
    if(shotList == undefined)
        return;
    
    //First step: erase scopePanels
    
    var shots = shotList.trim().split(',');
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        wavePanels[panelIdx].shots = shots;
        wavePanels[panelIdx].originalSignals = undefined;
        wavePanels[panelIdx].metrics = undefined;
        wavePanels[panelIdx].xMin = undefined;
        wavePanels[panelIdx].xMax = undefined;
        wavePanels[panelIdx].yMin = undefined;
        wavePanels[panelIdx].yMax = undefined;
        wavePanels[panelIdx].xIsDateTime = false;
        wavePanels[panelIdx].update();
    }
}

function titleEvent(event,response,expression)
{
    updateTitle(expression);
}

function titleEventFailure(event,response,expression) {}

function loadColorTables(colorTablesData)
{
    if(colorTablesData == undefined) return;
    var colorTablesLength = new Uint8Array(colorTablesData, 0, 1)[0];
    var colorTablesRGBData = new Uint8Array(colorTablesData, 1, colorTablesLength * 3 * 256);
    var colorNames = new Array();
    var offset = 1 + colorTablesLength * 3 * 256;
    for (var i = 0; i < colorTablesLength; i++)
    {
        var encodedColorName = new Uint8Array(colorTablesData, offset + 32 * i, 32);
        var colorName = '';
        for (var j = 0; j < 32; j++)
            colorName += String.fromCharCode(parseInt(encodedColorName[j]));
        colorNames.push(colorName.trim());
    }
    colorTables = new Object();
    for (var i = 0; i < colorTablesLength; i++)
    {
        var r = new Array(256);
        var g = new Array(256);
        var b = new Array(256);
        for (var j = 0; j < 256; j++)
        {
            r[j] = colorTablesRGBData[i * (256 * 3) + j];
            g[j] = colorTablesRGBData[i * (256 * 3) + 256 + j];
            b[j] = colorTablesRGBData[i * (256 * 3) + 256 * 2 + j];
        }
        colorTables[colorNames[i]] = {'r': r, 'g': g, 'b': b};
    }
}

function mdsScope(xmlDoc)
{
    document.oncontextmenu = contextMenu;
    var titleXml = xmlDoc.getElementsByTagName('title');
    if (titleXml.length != 0)
    {
      var expressionXml=titleXml[0].getElementsByTagName('expression');
      var expression=expressionXml[0].childNodes[0].nodeValue;
      updateTitle(expression);
      var eventXml=titleXml[0].getElementsByTagName('event');
      if (eventXml.length != 0)
      {
        var event=eventXml[0].childNodes[0].nodeValue;
        mdsplusEvent(event,true,titleEvent,titleEventFailure,expression);
      }
    }

//Get Color palette
    var paletteXml = xmlDoc.getElementsByTagName('palette');
    if(paletteXml.length != 0)
    {
        this.colorPalette = new Array();
        colorsXml = paletteXml[0].getElementsByTagName('color'); 
        for(var colorIdx = 0; colorIdx < colorsXml.length; colorIdx++)
            this.colorPalette.push(colorsXml[colorIdx].childNodes[0].nodeValue);
    }
    else
        this.colorPalette = ['black', 'red', 'blue', 'green'];

//Pass 1: build HTML table
    var htmlText = '<table><tr>';
    
    var columns = xmlDoc.getElementsByTagName('column');
    for(var colIdx = 0; colIdx < columns.length; colIdx++)
    {
        htmlText += '<td><table border = "1">';
        var rows = columns[colIdx].getElementsByTagName('panel');
        for(var rowIdx = 0; rowIdx < rows.length; rowIdx++)
        {
            htmlText += '<tr><td id="scope_'+rowIdx+'_'+colIdx+'">'+'</td></tr>';
        }
        htmlText += '</table></td>';
    }
    htmlText += '</tr></table>'+bottomLine;
    document.getElementById("ScopeArea").innerHTML=htmlText;

//Pass 2: create scope panels
    for(var colIdx = 0; colIdx < columns.length; colIdx++)
    {
        var panels = columns[colIdx].getElementsByTagName('panel');
        for(var panelIdx = 0; panelIdx < panels.length; panelIdx++)
        {
            var experiment = panels[panelIdx].getAttribute("tree");
            if ( experiment != null ) {
                var shot = panels[panelIdx].getAttribute("shot");
                  if (shot != null) {
                      var shots = new Array();
                      shots.push(parseInt(shot));
                  }
            }
            var signals = panels[panelIdx].getElementsByTagName("signal");
            var colors = new Array();
            var modes = new Array();
            var expressions = new Array();
            var limits = new Object();
            var isFramePanel = false;
            var setting;
            var event = null;
            var continuousUpdate = false;
            var panel=panels[panelIdx];
            if((setting=panel.getAttribute('xmin')) != null)
                limits.xMin = setting;
            if((setting=panel.getAttribute('xmax')) != null)
                limits.xMax = setting;
            if((setting=panel.getAttribute('ymin')) != null)
                limits.yMin = setting;
            if((setting=panel.getAttribute('ymax')) != null)
                limits.yMax = setting;
            if((setting=panel.getAttribute('is_image')) != null)
                isFramePanel = (setting == 'true');
            if(isFramePanel)
            {
                var frameSettings = new Object();
                if((setting=panel.getAttribute('keep_ratio')) != null)
                    frameSettings.keepRatio = (setting == 'true');
                if((setting=panel.getAttribute('horizontal_flip')) != null)
                    frameSettings.horizontalFlip = (setting == 'true');
                if((setting=panel.getAttribute('vertical_flip')) != null)
                    frameSettings.verticalFlip = (setting == 'true');
                if((setting=panel.getAttribute('palette')) != null)
                    frameSettings.palette = setting;
                if((setting=panel.getAttribute('bitShift')) != null)
                    frameSettings.bitShift = parseInt(setting);
                if((setting=panel.getAttribute('bitClip')) != null)
                    frameSettings.bitClip = (setting == 'true');
            }
            if((setting=panel.getAttribute('event')) != null)
                event = setting;
            if((setting=panel.getAttribute('continuous_update')) != null)
                continuousUpdate = (setting == '1');
            var labels = new Object();
            {
                if((setting=panel.getAttribute('title')) != null)
                    labels.title = setting;
                if((setting=panel.getAttribute('xlabel')) != null)
                    labels.xLabel = setting;
                if((setting=panel.getAttribute('ylabel')) != null)
                    labels.yLabel = setting;
            }
            for(var signalIdx = 0; signalIdx < signals.length; signalIdx++)
            {
                var color;
                if((setting=signals[signalIdx].getAttribute('color')) != null)
                    color = setting;
                else
                    color = 0;
                colors.push(this.colorPalette[color % this.colorPalette.length]);
                var mode;
                if((setting=signals[signalIdx].getAttribute('mode')) != null)
                    mode = parseInt(setting);
                else
                    mode = PLOT_LINE;
                modes.push(mode);
                var expression = signals[signalIdx].childNodes[0].nodeValue;
                expressions.push(expression);
            }
            var scopePanel;
            if(isFramePanel)
            {
                scopePanel = createFramePanel(document.getElementById('scope_'+panelIdx+'_'+colIdx),Math.round((window.innerWidth-40) /columns.length),
                    Math.round((window.innerHeight-100) /panels.length),columns.length, panels.length, colIdx, panelIdx, experiment, shots,
                    expressions, frameSettings, limits, labels);
            }
            else
            {
                scopePanel = createWavePanel(document.getElementById('scope_'+panelIdx+'_'+colIdx),Math.round((window.innerWidth-40) /columns.length),
                    Math.round((window.innerHeight-100) /panels.length),columns.length, panels.length, colIdx, panelIdx, experiment, shots,
                    expressions, colors, modes, limits, labels, continuousUpdate);
            }
            if(event)
                mdsplusEvent(event,true,eventUpdate,eventUpdateFail,scopePanel);
    
        }
    }
}

function startMdsScope(configName, shotName)
{
    var req = new XMLHttpRequest();
    if(shotName == '')
    	req.open('GET','?configxml=' + configName, true);
    else
    	req.open('GET','?configxml=' + configName+'&shot='+shotName, true);
    req.onreadystatechange = function() {
        if (this.readyState == 4) {
            if (this.getResponseHeader('ERROR'))
                alert('Error getting Scope configuration');
            else
            {
                var isConfig = this.getResponseHeader('IS_CONFIG');
                if(isConfig == 'YES')
                    mdsScope(this.responseXML);
                else
                    document.getElementById("ScopeArea").innerHTML = this.response;
            }
        }
    };
    req.send();

    var req2 = new XMLHttpRequest();
    req2.open('GET','?colortables=true', true);
    req2.onreadystatechange = function() {
        if (this.readyState == 4) {
            if (this.getResponseHeader('ERROR'))
                alert('Error getting color tables');
            else
                loadColorTables(this.response);
        }
    };
    req2.responseType = 'arraybuffer';
    req2.send();
}
