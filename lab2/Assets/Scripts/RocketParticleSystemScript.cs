using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using UnityEngine;

public class RocketParticleSystemScript : MonoBehaviour
{

    public Queue<GameObject> SpawnedParticles;
    private Queue<float> ParticleLifetimes;
    public GameObject ParticleSprite;
    public float SpawnRate; // [spawn/s]
    public float MaxLifetime = 1f; // [s]

    // Start is called before the first frame update
    void Start()
    {
        SpawnedParticles = new Queue<GameObject>();
        ParticleLifetimes = new Queue<float>();
        InvokeRepeating("SpawnParticles", 0, 1 / SpawnRate);
    }

    private void SpawnParticles()
    {
        // instantiate to copy original GameObject properties
        GameObject newParticle = Instantiate(ParticleSprite, transform.position, Quaternion.identity);
        // razzle dazzle za linearni smjer kretanja cestice
        newParticle.GetComponent<Rigidbody>().AddForce(new Vector3(Random.Range(-25f, 25f), 0f, Random.Range(-25f, 25f)));
        SpawnedParticles.Enqueue(newParticle);
        ParticleLifetimes.Enqueue(Time.time + Random.Range(-0.15f, 0.15f));
        //Debug.Log(Time.time);
    }

    
    // Update is called once per frame
    void Update()
    {
        float CurrentLifetime;
        if (this.ParticleLifetimes.Count > 0)
        {
            while (true)
            {
                CurrentLifetime = this.ParticleLifetimes.Peek();
                if (Time.time - CurrentLifetime > MaxLifetime)
                {
                    this.ParticleLifetimes.Dequeue();
                    //Destroy(lifetimeToDestroy) => primitive types cannot be destroyed?
                    GameObject particleToDestroy = this.SpawnedParticles.Dequeue();
                    Destroy(particleToDestroy);
                }
                else
                {
                    break;
                }
            }
        }  
    }
    
    
}
