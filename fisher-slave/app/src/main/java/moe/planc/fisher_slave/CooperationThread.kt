package moe.planc.fisher_slave

import android.graphics.PixelFormat
import android.hardware.display.VirtualDisplay
import android.media.ImageReader
import android.os.ConditionVariable
import android.os.Handler
import android.util.Log
import android.view.Display
import java.io.DataOutputStream
import java.net.InetAddress
import java.net.Socket

class CooperationThread(
    private val ip: String?,
    private val port: Int,
    private var width: Int,
    private var height: Int,
    private var display: Display,
    private var virtualDisplay: VirtualDisplay
    ) : Thread() {

    private val TAG = "CooperationThread"
    private var handler: Handler = Handler()
    private var imageReader: ImageReader
    private var listener: ImageAvailableListener
    private lateinit var socket: Socket
    private var condition = ConditionVariable(false)
    private var running = true
    private var rotation = 0

    init {
        listener = ImageAvailableListener(condition)
        imageReader = ImageReader.newInstance(width, height, PixelFormat.RGBA_8888, 2)
        imageReader.setOnImageAvailableListener(listener, handler)
        virtualDisplay.surface = imageReader.surface

    }

    private class ImageAvailableListener(private var condition: ConditionVariable) : ImageReader.OnImageAvailableListener {
        override fun onImageAvailable(p0: ImageReader?) {
            condition.open()
        }
    }

    override fun run() {
            socket = Socket(InetAddress.getByName(ip), port)
            val out = DataOutputStream(socket.getOutputStream())

            running = true
            while (running) {
                    condition.block()
                    condition.close()
                    var image = imageReader.acquireLatestImage() ?: continue
                try {
                    val planes = image.planes
                    val buffer = planes[0].buffer
                    val byteArray = ByteArray(planes[0].rowStride * image.height)
                    buffer.get(byteArray)

                    out.writeInt(planes[0].rowStride)
                    out.writeInt(image.width)
                    out.writeInt(image.height)
                    out.write(byteArray)
                } catch (e: Exception) {
                    Log.e(TAG, e.toString())
                }

                image.close()
                resize()
            }

        if (socket != null) {
            socket.close()
        }
    }

    fun exit() {
        running = false
        condition.open()
    }

    fun resize() {
        if (display?.rotation != rotation) {
            if ((display?.rotation == 1 || display?.rotation == 3)) {
                virtualDisplay.resize(height, width, 1)
                imageReader.close()
                imageReader = ImageReader.newInstance(height, width, PixelFormat.RGBA_8888, 2)
                imageReader.setOnImageAvailableListener(listener, handler)
                virtualDisplay.surface = imageReader.surface
            } else {
                virtualDisplay.resize(width, height, 1)
                imageReader = ImageReader.newInstance(width, height, PixelFormat.RGBA_8888, 2)
                imageReader.setOnImageAvailableListener(listener, handler)
                virtualDisplay.surface = imageReader.surface
            }
            rotation = display?.rotation!!
        }
    }
}