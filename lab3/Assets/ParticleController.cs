using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ParticleController : MonoBehaviour
{

    private float currentTeddyY;
    private TeddyController teddyController;

    // Start is called before the first frame update
    void Start()
    {
        teddyController = GameObject.Find("teddy").GetComponent<TeddyController>();
        currentTeddyY = teddyController.GetMaxY();
        Debug.Log("Start particle position: " + transform.position.ToString());
    }

    private void OnEnable()
    {
        teddyController = GameObject.Find("teddy").GetComponent<TeddyController>();
        currentTeddyY = teddyController.GetMaxY();
        Debug.Log("OnEnable particle position: " + transform.position.ToString());
    }

    // Update is called once per frame
    void Update()
    {
        
        if (gameObject.transform.position.y < currentTeddyY - 10f) 
        {
            gameObject.SetActive(false);
        }
    }
}
