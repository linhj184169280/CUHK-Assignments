using UnityEngine;
using System.Collections;

public class EnermyScript : MonoBehaviour {
	public Transform spacecraft;
	private float HealthPoint;
	Rigidbody rb;
	// Use this for initialization
	void Start () {
		HealthPoint = 500.0f;
	}
	
	// Update is called once per frame
	void Update () {
		float distance = (transform.position - spacecraft.position).magnitude;
		if (distance > 50.0f) {
			transform.position = Vector3.MoveTowards (transform.position, spacecraft.position, Time.deltaTime * 10.0f);
		}
		if (distance > 20.0f && distance <= 50.0f) {
			transform.position = Vector3.MoveTowards (transform.position, spacecraft.position, Time.deltaTime * 5.0f);
		}
		if ( distance <= 20.0f) {
			transform.position = Vector3.MoveTowards (transform.position, spacecraft.position, Time.deltaTime * 2.0f);
		}

	}
	void SetSpacecraft(Transform m_spacecraft){
		spacecraft = m_spacecraft;
	}
	void OnTriggerEnter(Collider other){
		if (other.gameObject.CompareTag ("BulletTag")) {
			other.gameObject.SetActive (false);
			HealthPoint = HealthPoint - 10.0f;
			//gameObject.SetActive (false);
			if (HealthPoint <= 0.0f) {
				rb = GetComponent<Rigidbody> ();
				rb.mass = 10.0f;
				Destroy (gameObject, 3.0f);
			}
		}
		if (other.gameObject.CompareTag ("SpacecraftTag")) {
			Destroy (gameObject);
		}
	}
}
