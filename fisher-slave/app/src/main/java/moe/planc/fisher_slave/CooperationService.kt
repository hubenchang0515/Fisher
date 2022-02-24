package moe.planc.fisher_slave

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
import android.graphics.PixelFormat
import android.graphics.Point
import android.media.ImageReader
import android.os.Build
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import java.net.InetAddress
import kotlin.Exception
import android.hardware.display.VirtualDisplay
import java.io.DataOutputStream
import android.view.Display
import android.view.WindowManager





class CooperationService : Service() {
    private val TAG = "CooperationService"
    private lateinit var innerThread: CooperationThread
    private lateinit var projectionManager: MediaProjectionManager
    private lateinit var projection: MediaProjection
    private val pid = android.os.Process.myPid();

    override fun onBind(intent: Intent): IBinder {
        TODO("Return the communication channel to the service.")
    }

    override fun onDestroy() {
        try {
            innerThread.exit()
            super.onDestroy()
        } catch (e:Exception) {
            Log.e(TAG, e.toString())
        }
    }

    @RequiresApi(Build.VERSION_CODES.R)
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

            projectionManager = getSystemService(MEDIA_PROJECTION_SERVICE) as MediaProjectionManager
            projection = projectionManager.getMediaProjection(resultCode, intent)

            val window = getSystemService(WINDOW_SERVICE) as WindowManager
            val display = window.defaultDisplay

            innerThread = CooperationThread(ip, port, projection, display!!)
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