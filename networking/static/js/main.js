require.config({
    shim: {
        three: { exports: 'THREE' },
        posit2: { exports: 'POS' },
        aruco: { exports: 'AR' },
        posit1: { exports: 'POS' },
        svd: { exports: 'SVD' },
        cv: { exports: 'CV' }
    }
});

requirejs(["choreographer", "monoscreen", "comm", "tracking"], function(choreographer, monoscreen, comm, tracking) {
    var FRAMERATE = 15;
    var pose;

    var views = choreographer.setup();
    document.body.appendChild(views.observer.renderer.domElement);
    document.body.appendChild(views.device.renderer.domElement);

    function addHiddenElement (el) {
        el.style.visibility = "hidden";
        el.style.position = "absolute";
        document.body.appendChild(el);
    }

    function sendImage () {
        var ditheredImage = monoscreen.render(choreographer.getImage(views.device.renderer));
        comm.send(ditheredImage);
    }

    function animate () {
        var pose = tracking.getPose();

        if (pose) {
            var t, rotation;

            if (pose.bestError > pose.alternativeError) {
                t = pose.alternativeTranslation;
                rotation = pose.alternativeRotation;
            } else {
                t = pose.bestTranslation;
                rotation = pose.bestRotation;
            }

            var rx = Math.asin(-rotation[1][2]);
            var ry = -Math.atan2(rotation[0][2], rotation[2][2]);
            var rz = Math.atan2(rotation[1][0], rotation[1][1]);

            var camera = views.device.camera;

            camera.position.x = -t[0];
            camera.position.y = t[1];
            camera.position.z = t[2] / 2;

            views.observer.cameraProp.position.x = -t[0];
            views.observer.cameraProp.position.y = t[1];
            views.observer.cameraProp.position.z = t[2] / 2;

            var lookVector = new THREE.Vector3(0, 0, -1);
            lookVector.applyEuler(new THREE.Euler(rx, ry, rz, "XYZ"));
            lookVector.add(camera.position);

            camera.lookAt(lookVector);
            views.observer.cameraProp.lookAt(lookVector);
        }

        // render
        requestAnimationFrame(animate);
        views.device.render();
        views.observer.render();
    }

    (function () {
        var cam = document.getElementById("camera");
        var canvas = tracking.setup(cam);
        //addHiddenElement(canvas);
        document.body.appendChild(canvas);

        var commInterval = setInterval(function () {
            sendImage();
        }, 1000 / FRAMERATE);

        animate();
    })();
});
