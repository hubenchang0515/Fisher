package moe.planc.fisher_slave

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.EditText

class MainActivity : AppCompatActivity() {
    lateinit var ipEditText: EditText
    lateinit var portEditText: EditText
    lateinit var connectButton: Button
    lateinit var scanButton: Button

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        ipEditText = findViewById(R.id.ip_edit_text)
        portEditText = findViewById(R.id.port_edit_text)
        connectButton = findViewById(R.id.connect_button)
        scanButton = findViewById(R.id.scan_button)

        connectButton.setOnClickListener {
            val ip = ipEditText.text.toString()
            val port = portEditText.text.toString().toIntOrNull() ?: 0
            Log.d("MainActivity", "connect to ip:[$ip] port:[$port]")

            val intent = Intent(this, CooperationActivity::class.java)
            intent.putExtra("ip", ip)
            intent.putExtra("port", port)
            startActivity(intent)
        }
    }
}