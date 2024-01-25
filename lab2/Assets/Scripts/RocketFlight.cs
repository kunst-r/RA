using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RocketFlight : MonoBehaviour
{
    public float RocketAcceleration = 0.05f;
    public float MaxRocketSpeed = 200;
    public float RocketSpeed = 0f;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if (RocketSpeed < MaxRocketSpeed)
        {
            RocketSpeed += RocketAcceleration;
            this.GetComponent<Rigidbody>().AddForce(0f, RocketAcceleration, 0f);
        }
        
    }
}
