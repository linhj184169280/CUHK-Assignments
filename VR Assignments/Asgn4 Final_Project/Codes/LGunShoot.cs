using UnityEngine;
using System.Collections;

public class LGunShoot : MonoBehaviour {
	public Rigidbody projectile;
	private float speed = 100.0f;

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
		if (Input.GetMouseButton(0) ) {
			Rigidbody clone = (Rigidbody)Instantiate (projectile, transform.position, transform.rotation);
			clone.velocity = transform.TransformDirection (new Vector3 (0.0f, 0.0f,speed));

			Destroy (clone.gameObject, 0.5f);
		}
	}
}
