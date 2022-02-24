package moe.planc.fisher_slave

import android.graphics.Bitmap
import android.graphics.PixelFormat
import android.graphics.Point
import android.hardware.display.DisplayManager
import android.hardware.display.VirtualDisplay
import android.media.ImageReader
import android.media.projection.MediaProjection
import android.os.ConditionVariable
import android.os.Handler
import android.util.Log
import android.view.Display
import java.io.ByteArrayOutputStream
import java.io.DataOutputStream
import java.net.InetAddress
import java.net.Socket
import kotlin.experimental.and
import kotlin.experimental.or

class CooperationThread(
    private val ip: String?,
    private val port: Int,
    private val projection: MediaProjection,
    private val display: Display,
    ) : Thread() {

    private val TAG = "CooperationThread"
    private val handler: Handler
    private val virtualDisplay: VirtualDisplay
    private var imageReader: ImageReader
    private val listener: ImageAvailableListener
    private val condition = ConditionVariable(false)
    private var running = true
    private var rotation = 0
    private var width = 0
    private var height = 0
    private lateinit var socket: Socket

    init {
        val size = Point()
        display.getRealSize(size)
        width = (size.x / 2.5).toInt()
        height = (size.y / 2.5).toInt()
        handler = Handler()
        listener = ImageAvailableListener(condition)
        imageReader = ImageReader.newInstance(width, height, PixelFormat.RGBA_8888, 2)
        imageReader.setOnImageAvailableListener(listener, handler)
        virtualDisplay = projection.createVirtualDisplay(
            "cooperation",
            width,
            height,
            1,
            DisplayManager.VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR,
            null,
            null,
            null
        )
        virtualDisplay.surface = imageReader.surface
    }

    private class ImageAvailableListener(private var condition: ConditionVariable) : ImageReader.OnImageAvailableListener {
        override fun onImageAvailable(p0: ImageReader?) {
            condition.open()
        }
    }

    override fun run() {
            try {
                socket = Socket(InetAddress.getByName(ip), port)
            } catch (e:Exception) {
                Log.e(TAG, e.toString())
                return
            }

            val out = DataOutputStream(socket.getOutputStream())

            running = true
            while (running) {
                    condition.block()
                    condition.close()
                    var image = imageReader.acquireLatestImage() ?: continue
                try {
                    val planes = image.planes
                    val buffer = planes[0].buffer
                    var bitmap = Bitmap.createBitmap(planes[0].rowStride / 4, image.height, Bitmap.Config.ARGB_8888)
                    bitmap.copyPixelsFromBuffer(buffer)
                    bitmap = Bitmap.createBitmap(bitmap, 0, 0, image.width, image.height);

                    var byteArrayOutputStream = ByteArrayOutputStream()
                    bitmap.compress(Bitmap.CompressFormat.JPEG, 85, byteArrayOutputStream)
                    byteArrayOutputStream.flush()
                    out.writeInt(image.width)
                    out.writeInt(image.height)
                    out.writeInt(byteArrayOutputStream.size())
                    out.write(byteArrayOutputStream.toByteArray())
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