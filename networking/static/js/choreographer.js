if (typeof define !== 'function') {
    var define = require('amdefine')(module);
}

define(function(require) {
    var exports = {};

    var THREE = require("three");
    var texFragShader = require("text!../shaders/texture.fs");
    var texVertShader = require("text!../shaders/texture.vs");

    var observer, player;

    function Observer () {
        this.renderer = new THREE.WebGLRenderer();
        this.scene = new THREE.Scene();

        this.camera = new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 1, 10000);
        this.camera.lookAt(new THREE.Vector3(0, 0, 0));

        this.renderer.setSize(window.innerWidth, window.innerHeight);

        var cam = this.camera;
        function handleMouseMove(event) {
            var mouseX = event.pageX / window.innerWidth;
            var mouseY = event.pageY / window.innerHeight;

            var theta = mouseX * 2 * Math.PI - Math.PI;
            var phi = mouseY * 2 * Math.PI;

            var radius = 300;
            cam.position.x = radius * Math.sin(theta) * Math.cos(phi);
            cam.position.y = radius * Math.sin(phi);
            cam.position.z = radius * Math.cos(theta) * Math.cos(phi);
            cam.updateMatrix();
            cam.lookAt(new THREE.Vector3(0, 0, 0));
        }

        function makeCamera () {
            var amberMaterial = new THREE.MeshBasicMaterial({color: 0xffc000});
            var blueMaterial = new THREE.MeshBasicMaterial({color: 0x0000ff});

            var body = new THREE.Mesh(new THREE.BoxGeometry(20, 20, 10), amberMaterial);
            var lens = new THREE.Mesh(new THREE.CylinderGeometry(5, 5, 10, 32), blueMaterial);
            lens.rotation.x = Math.PI / 2;
            lens.position.z = 10;

            var button = new THREE.Mesh(new THREE.CylinderGeometry(1, 1, 1, 32), blueMaterial);
            button.rotation.x = Math.PI / 2;
            button.position.x = -5;
            button.position.y = -10;

            body.add(lens);
            body.add(button);

            return body;
        }

        var ambientLight = new THREE.AmbientLight(0x555555);
        this.scene.add(ambientLight);

        var directionalLight = new THREE.DirectionalLight(0xffffff);
        directionalLight.position.set(1, 1, 1).normalize();
        this.scene.add(directionalLight);

        this.cameraProp = makeCamera();
        this.scene.add(this.cameraProp);

        var amberMaterial = new THREE.MeshBasicMaterial({color: 0xff0000});
        var origin = new THREE.Mesh(new THREE.BoxGeometry(10, 10, 10));
        origin.position.x = 50;
        this.scene.add(origin);

        document.onmousemove = handleMouseMove;
    }

    Observer.prototype.render = function () {
        this.renderer.render(this.scene, this.camera);
    }

    function Device () {
        this.renderer = new THREE.WebGLRenderer();
        this.scene = new THREE.Scene();

        this.camera = new THREE.PerspectiveCamera(45, 96 / 64, 1, 10000);
        this.camera.position.z = 100;
        this.camera.lookAt(new THREE.Vector3(0, 0, 0));

        this.renderer.setSize(94, 64);

        var loader = new THREE.JSONLoader();

        var ambientLight = new THREE.AmbientLight(0x555555);
        this.scene.add(ambientLight);

        var directionalLight = new THREE.DirectionalLight(0xffffff);
        directionalLight.position.set(1, 1, 1).normalize();
        this.scene.add(directionalLight);

        loader.load("models/cube-blended.json", (function(geometry, materials) {
            var material = new THREE.ShaderMaterial({
                uniforms: {
                    time: { type: "f", value: 1.0 },
                    resolution: { type: "v2", value: new THREE.Vector2() },
                    texture1: { type: "t", value: materials[0].map }
                },
                vertexShader: texVertShader,
                fragmentShader: texFragShader
            });

            mesh = new THREE.Mesh(geometry, material);
            mesh.scale.set(10, 10, 10);
            mesh.position.x = 50;
            mesh.position.y = 0;
            mesh.position.z = 0;

            this.scene.add(mesh);
        }).bind(this));
    }

    Device.prototype.render = function () {
        this.renderer.render(this.scene, this.camera);
    }

    exports.setup = function () {
        return {
            observer: new Observer(),
            device: new Device()
        };
    };

    exports.getImage = function (targetRenderer) {
        targetRenderer = targetRenderer;

        var width = targetRenderer.domElement.width;
        var height = targetRenderer.domElement.height;
        var pixels = new Uint8Array(4 * width * height);

        var gl = targetRenderer.getContext();
        gl.readPixels(0, 0, width, height, gl.RGBA, gl.UNSIGNED_BYTE, pixels);

        return new ImageData(new Uint8ClampedArray(pixels), width, height);
    }

    return exports;
});
