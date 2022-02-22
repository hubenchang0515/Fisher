package moe.planc.fisher_slave

import android.content.Intent
import android.hardware.display.DisplayManager
import android.media.projection.MediaProjectionManager
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.Toast

class CooperationActivity : AppCompatActivity() {
    private lateinit var projectionManager: MediaProjectionManager
    private lateinit var projectionData: Intent
    private lateinit var serviceIntent: Intent
    private var ip = ""
    private var port = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_cooperation)

        ip = intent.getStringExtra("ip").toString()
        port = intent.getIntExtra("port", 0)

        projectionManager = getSystemService(MEDIA_PROJECTION_SERVICE) as MediaProjectionManager
        startActivityForResult(projectionManager.createScreenCaptureIntent(), 1)
    }

    override fun onDestroy() {
        super.onDestroy()
        if (serviceIntent != null) {
            stopService(serviceIntent)
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)

        if (resultCode != RESULT_OK || data == null) {
            Toast.makeText(this, "failed", Toast.LENGTH_SHORT).show()
            return
        }

        projectionData = data
        serviceIntent = Intent(this, CooperationService::class.java)
        serviceIntent.putExtra("ip", ip)
        serviceIntent.putExtra("port", port)
        serviceIntent.putExtra("resultCode", resultCode)
        serviceIntent.putExtras(data)
        startService(serviceIntent)
    }
}