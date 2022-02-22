package moe.planc.fisher_slave

import android.R.attr
import android.app.*
import android.content.Intent
import android.hardware.display.DisplayManager
import android.media.projection.MediaProjection
import android.media.projection.MediaProjectionManager
import android.os.IBinder
import android.util.Log
import java.net.Socket
import androidx.core.app.NotificationCompat

import android.content.Context

import android.graphics.Color
import android.graphics.ImageFormat
import android.graphics.PixelFormat
import android.media.Image
import android.media.ImageReader
import android.os.Build
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import java.net.InetAddress
import kotlin.Exception
import android.graphics.Bitmap

import android.R.attr.bitmap
import android.media.Image.Plane
import java.io.ByteArrayOutputStream
import java.io.DataOutputStream
import java.lang.StringBuilder
import java.nio.ByteBuffer


class CooperationService : Service() {
    private val TAG = "CooperationService"
    private lateinit var innerThread: InnerThread
    private lateinit var projectionManager: MediaProjectionManager
    private lateinit var projection: MediaProjection
    private lateinit var imageReader: ImageReader
    private val pid = android.os.Process.myPid();
    private val width = 432
    private val height = 960

    private class InnerThread(val ip: String?, val port: Int, val imageReader: ImageReader) : Thread() {
        private val TAG = "CooperationService"
        private lateinit var socket: Socket
        private val width = 432
        private val height = 960
        private var running = true

        override fun run() {
            try {
                socket = Socket(InetAddress.getByName(ip), port)
                val out = DataOutputStream(socket.getOutputStream())
                running = true
                while (running) {
                    var image = imageReader.acquireLatestImage() ?: continue
                    val planes = image.planes
                    val buffer = planes[0].buffer
                    val byteArray = ByteArray(planes[0].rowStride * height)
                    buffer.get(byteArray)
                    out.writeInt(planes[0].rowStride)
                    out.writeInt(width)
                    out.writeInt(height)
                    out.write(byteArray)
                    image.close()
                }
            } catch (e:Exception) {
                Log.e(TAG, e.toString())
            }

            socket.close()
        }

        fun exit() {
            running = false
        }
    }

    override fun onBind(intent: Intent): IBinder {
        TODO("Return the communication channel to the service.")
    }

    override fun onDestroy() {
        innerThread.exit()
        super.onDestroy()
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        if (intent == null) {
            return START_NOT_STICKY
        }

        try {
            startForeground(pid, createNotification());

            val ip = intent.getStringExtra("ip")
            val port = intent.getIntExtra("port", 0)
            Log.d(TAG, "connect to $ip:$port")

            val resultCode = intent.getIntExtra("resultCode", AppCompatActivity.RESULT_OK)
            imageReader = ImageReader.newInstance(432, 960, PixelFormat.RGBA_8888, 2)

            projectionManager = getSystemService(MEDIA_PROJECTION_SERVICE) as MediaProjectionManager
            projection = projectionManager.getMediaProjection(resultCode, intent)
            projection.createVirtualDisplay(
                "cooperation",
                width,
                height,
                1,
                DisplayManager.VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR,
                imageReader.surface,
                null,
                null
            )

            innerThread = InnerThread(ip, port, imageReader)
            innerThread.start()
        } catch (e:Exception) {
            Log.e("CooperationActivity", e.toString())
        }

        return super.onStartCommand(intent, flags, startId)
    }

    @RequiresApi(Build.VERSION_CODES.O)
    private fun createNotificationChannel(channelId: String, channelName: String): String {
        val channel = NotificationChannel(channelId,
            channelName, NotificationManager.IMPORTANCE_NONE)
        channel.lightColor = Color.BLUE
        channel.lockscreenVisibility = Notification.VISIBILITY_PRIVATE
        val service = getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
        service.createNotificationChannel(channel)
        return channelId
    }

    private fun createNotification(): Notification? {
        val channelId =
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            createNotificationChannel("CooperationService-Notification", "recording")
        } else {
            ""
        }

        val intent = Intent(this, MainActivity::class.java)
        val pendingIntent = PendingIntent.getActivity(
            this, 0, intent,
            PendingIntent.FLAG_UPDATE_CURRENT
        )
        val builder = NotificationCompat.Builder(this, channelId)
            .setContentTitle("recording screen")
            .setContentText("recording screen for cooperation")
            .setPriority(NotificationCompat.PRIORITY_MAX)
            .setWhen(System.currentTimeMillis())
            .setDefaults(NotificationCompat.DEFAULT_ALL)
            .setContentIntent(pendingIntent)
        val notification: Notification = builder.build()
        notification.flags = Notification.FLAG_AUTO_CANCEL
        return notification
    }
}