// https://codepen.io/jlfwong/pen/xgwZzd

var cmds = [ { type: 'M', x: 358, y: 109 },
     { type: 'L', x: 370, y: 97 },
     { type: 'Q', x1: 326, y1: 46, x: 271, y: 17.5 },
     { type: 'Q', x1: 216, y1: -11, x: 163, y: -11 },
     { type: 'Q', x1: 104, y1: -11, x: 67.5, y: 27 },
     { type: 'Q', x1: 31, y1: 65, x: 31, y: 126 },
     { type: 'Q', x1: 31, y1: 199, x: 76, y: 273 },
     { type: 'Q', x1: 121, y1: 347, x: 191.5, y: 394 },
     { type: 'Q', x1: 262, y1: 441, x: 332, y: 441 },
     { type: 'Q', x1: 369, y1: 441, x: 390.5, y: 423 },
     { type: 'Q', x1: 412, y1: 405, x: 412, y: 373 },
     { type: 'Q', x1: 412, y1: 308, x: 340, y: 257 },
     { type: 'Q', x1: 268, y1: 206, x: 156, y: 190 },
     { type: 'L', x: 128, y: 186 },
     { type: 'Q', x1: 118, y1: 146, x: 118, y: 124 },
     { type: 'Q', x1: 118, y1: 83, x: 143.5, y: 58.5 },
     { type: 'Q', x1: 169, y1: 34, x: 212, y: 34 },
     { type: 'Q', x1: 244, y1: 34, x: 274, y: 49.5 },
     { type: 'Q', x1: 304, y1: 65, x: 358, y: 109 },
     { type: 'M', x: 135, y: 208 },
     { type: 'Q', x1: 189, y1: 221, x: 229.5, y: 240 },
     { type: 'Q', x1: 270, y1: 259, x: 291.5, y: 277 },
     { type: 'Q', x1: 313, y1: 295, x: 326, y: 316 },
     { type: 'Q', x1: 339, y1: 337, x: 342.5, y: 351 },
     { type: 'Q', x1: 346, y1: 365, x: 346, y: 379 },
     { type: 'Q', x1: 346, y1: 396, x: 336.5, y: 407 },
     { type: 'Q', x1: 327, y1: 418, x: 311, y: 418 },
     { type: 'Q', x1: 273, y1: 418, x: 225, y: 368.5 },
     { type: 'Q', x1: 177, y1: 319, x: 152, y: 252 },
     { type: 'L', x: 135, y: 208 }];

var curveSpaceToPixelSpace = function(pt) {
  return [
    0.065 * pt[0],
    -0.065 * pt[1] + 30
  ];
};

var B = function(curve, t) {
  var [[x0, y0], [x1, y1], [x2, y2]] = curve;
  return [
    (1-t) * (1-t) * x0 + 2 * (1 - t) * t * x1 + t * t * x2,
    (1-t) * (1-t) * y0 + 2 * (1 - t) * t * y1 + t * t * y2    
  ]
}

var Bprime = function(curve, t) {
  var [[x0, y0], [x1, y1], [x2, y2]] = curve;
  return [
    (-2 + 2 * t) * x0 + (2 - 4*t) * x1 + 2 * t * x2,
    (-2 + 2 * t) * y0 + (2 - 4*t) * y1 + 2 * t * y2,
  ];
};

var intersectCurveAndScanline = function(curve, ys) {
  var [[x0, y0], [x1, y1], [x2, y2]] = curve;
  var a = y2 - 2*y1 + y0;
  var b = 2*y1 - 2*y0;
  var c = y0 - ys;
  var discriminant = b*b - 4 * a * c;
  
  if (a === 0) {
    var t = [-c/b];
    if (t >= 0 && t <= 1) {
      return [-c/b];
    } else {
      return [];
    }
  }
  
  if (discriminant < 0) {
    return [];
  }
  var ts = [];
  
  var sqrtDiscriminant = Math.sqrt(discriminant);
  var t1 = (-b - sqrtDiscriminant) / (2 * a);
  if (t1 >= 0 && t1 <= 1) {
    ts.push(t1);
  }
  
  if (discriminant > 0) {
    var t2 = (-b + sqrtDiscriminant) / (2 * a);
    if (t2 >= 0 && t2 <= 1) {
      ts.push(t2);
    }
  }
  
  return ts;
};

var curves = [];

var p0, p1, p2;
cmds.forEach(c => {
  if (c.type == 'L') {
    p1 = curveSpaceToPixelSpace([c.x, c.y]);
    p2 = curveSpaceToPixelSpace([c.x, c.y]);
    curves.push([p0, p1, p2]);
  } else if (c.type == 'Q') {
    p1 = curveSpaceToPixelSpace([c.x1, c.y1]);
    p2 = curveSpaceToPixelSpace([c.x, c.y]);
    curves.push([p0, p1, p2]);
  }
  p0 = curveSpaceToPixelSpace([c.x, c.y]);
});

var draw = function(options) {
  var height = 32;
  var width = 32;  
  
  var {drawCurves, drawIntersections, drawCenters, maxY, drawControls} = options;
  if (!maxY) maxY = 0;
  
  var svg = document.getElementById("svg");
  svg.removeChild(svg.firstChild);
  var xmlns = "http://www.w3.org/2000/svg";
  var g = document.createElementNS(xmlns, "g");
  svg.appendChild(g);
 
  var addCircle = function(cx, cy, r, fill) {
    var c = document.createElementNS(xmlns, "circle");
    c.setAttribute("cx", cx);
    c.setAttribute("cy", cy);
    c.setAttribute("r", r);
    c.setAttribute("fill", fill);
    g.appendChild(c);
  }
  
  var activeCurves = {};

  for (var y = 0; y < height; y++) {
    // Intersections is [[x, y'], ...]
    var ys = y + 0.5;
    var intersections = curves.reduce((sofar, curve, i) => {
      var ts = intersectCurveAndScanline(curve, ys);
      if (y === maxY && ts.length > 0) {
        activeCurves[i] = true;
      }
      return sofar.concat(ts.map(t => [B(curve, t)[0], Bprime(curve, t)[1]]));
    }, []);

    for (var x = 0; x < width; x++) {
      var windingNumber = intersections.reduce((sofar, [xInter, yPrime]) => {
        if (xInter > (x + 0.5)) {
          return sofar + (yPrime > 0 ? -1 : 1)
        } else {
          return sofar;
        }
      }, 0);
      
      var fill = 'none';
      if (y == maxY) {
        fill = '#F2C94C';
      } else if (y < maxY) {
        if (windingNumber != 0) {
          fill = 'black';
        }
      }
      
      var r = document.createElementNS(xmlns, "rect");
      r.setAttribute("x", x);
      r.setAttribute("y", y);
      r.setAttribute("width", 1);
      r.setAttribute("height", 1);
      r.setAttribute("stroke", "#888");
      r.setAttribute("fill", fill);
      r.setAttribute("stroke-width", drawCenters ? "0.05" : 0);
      g.appendChild(r);
      
     
      if (drawCenters && y < maxY) {
        addCircle(x+0.3, y + 0.3, 0.1, (windingNumber === 0) ? '#ccc' : 'white');
        addCircle(x+0.3, y + 0.7, 0.1, (windingNumber === 0) ? '#ccc' : 'white');
        addCircle(x+0.7, y + 0.3, 0.1, (windingNumber === 0) ? '#ccc' : 'white');
        addCircle(x+0.7, y + 0.7, 0.1, (windingNumber === 0) ? '#ccc' : 'white');
      }
    }
    
    if (drawIntersections && y === maxY) {
      var p = document.createElementNS(xmlns, "path");
      p.setAttribute("d", `M 0 ${ys} L ${width} ${ys}`);
      p.setAttribute("stroke", "#219653");
      p.setAttribute("stroke-width", "0.15");
      g.appendChild(p);
      intersections.forEach(([xi, ypi]) => {
        addCircle(xi, ys, 0.3, '#2F80ED');
      });
    }
  }
  


  
  if (drawCurves) {
    curves.forEach((curve, i) => {
      var [[x0, y0], [x1, y1], [x2, y2]] = curve;  
      var p = document.createElementNS(xmlns, "path");
      var stroke = i % 2 ? '#4F4F4F' : '#BDBDBD';
      if (activeCurves[i]) {
        stroke = '#EB5757';
      }
      p.setAttribute("d", `M ${x0} ${y0}Q ${x1} ${y1} ${x2} ${y2}`);
      p.setAttribute("stroke", stroke);
      p.setAttribute("stroke-width", "0.1");
      p.setAttribute("fill", "none");
      g.appendChild(p);
    })
  }
  
  if (drawControls) {
    curves.forEach((curve, i) => {
      var [[x0, y0], [x1, y1], [x2, y2]] = curve;               
      var p = document.createElementNS(xmlns, "path");
      p.setAttribute("d", `M ${x0} ${y0} L ${x1} ${y1} L ${x2} ${y2}`);
      p.setAttribute("stroke", "black");
      p.setAttribute("stroke-width", "0.1");
      p.setAttribute("stroke-dasharray", "0.1 0.2");
      p.setAttribute("fill", "none");
      g.appendChild(p);
      addCircle(x0, y0, 0.2, '#EB5757');
      addCircle(x1, y1, 0.2, '#2F80ED');

    });
  }


  /*
  curves.forEach((curve) => {
    var [[x0, y0], [x1, y1], [x2, y2]] = curve;
    addCircle(x0, y0, 0.3, "red"); 
  });
  */
};


var i = 1;
(function tick() {
  draw({drawCurves: true, drawCenters: true, maxY: i, drawIntersections: true});
  i += 1;
  i = i % 36;
  setTimeout(tick, 200);
})();


/*
draw({drawCurves: true, maxY: 33, drawControls: false, drawCenters: true});
*/