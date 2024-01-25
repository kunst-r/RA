using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class CrushablePlatformController : MonoBehaviour
{

    public GameObject particlePrefab;
    public int spawnedParticleCount = 10;

    private Queue<GameObject> spawnedParticles;

    private void Start()
    {
        spawnedParticles = new Queue<GameObject>();
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (!collision.gameObject.CompareTag("Player"))
        {
            return;
        }

        // if we fall onto the red platform, spawn particles and set the platform inactive
        if (collision.gameObject.GetComponent<Rigidbody2D>().velocity.y <= 0f)
        {
            Debug.Log("crushed platform position: " + transform.position.x + ", " + transform.position.y + ", " + transform.position.z);
            for (int i = 0; i < spawnedParticleCount; ++i)
            {
                Vector2 randomForceVector = new Vector2(Random.Range(-300f, 300f), Random.Range(100f, 300f));
                Debug.Log("random force vector = " + randomForceVector.ToString());
                GameObject newParticle = Instantiate(particlePrefab, transform.position, Quaternion.identity);
                newParticle.SetActive(true);
                newParticle.GetComponent<Rigidbody2D>().totalForce = new Vector2(0f, 0f);
                newParticle.GetComponent<Rigidbody2D>().AddForce(randomForceVector);
                spawnedParticles.Enqueue(newParticle);
            }

            gameObject.SetActive(false);
            
        }
    }
}
